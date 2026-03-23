# Usage: python train_dnn1.py sipmHits.csv anniEvents.csv neutroncaptureEvents.csv
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import joblib

from sklearn.preprocessing import MinMaxScaler
from sklearn.model_selection import train_test_split
from sklearn.utils import shuffle
import sys
import tensorflow as tf
from tensorflow.keras import layers, models

def _parse_time_cell_to_float(x) -> float:
    try:
        s = str(x).strip().replace(",", " ")
        tok = s.split()
        return float(tok[0]) if tok else np.nan
    except:
        return np.nan


def load_hits_raw(hits_csv: str) -> pd.DataFrame:
    df = pd.read_csv(hits_csv)
    
    col_map = {
        "Sipm_Hit_XPosition": "X(mm)",
        "Sipm_Hit_YPosition": "Y(mm)",
        "Sipm_Hit_ZPosition": "Z(mm)",
        "SipmTime(ns)": "Time(ns)"
    }
    df = df.rename(columns=col_map)
    
    # apply the time parser to handle the double-value column
    df["Time(ns)"] = df["Time(ns)"].apply(_parse_time_cell_to_float).astype("float32")
    
    required = ["FileIndex", "EventID", "X(mm)", "Y(mm)", "Z(mm)", "Time(ns)"]
    return df[required].dropna()

def load_labels_one(labels_csv: str) -> pd.DataFrame:
    lab = pd.read_csv(labels_csv)
    
    col_map = {
        "X(mm)": "tx",
        "Y(mm)": "ty",
        "Z(mm)": "tz",
        "Time(ns)": "tt"
    }
    lab = lab.rename(columns=col_map)
    
    # ensure time is float
    lab["tt"] = lab["tt"].apply(_parse_time_cell_to_float).astype("float32")
    
    return lab[["FileIndex", "EventID", "tx", "ty", "tz", "tt"]].dropna()

def hits_info (hits, ann, neu):
    # filtering sipmhits, annihil and neutroncapture events
    # filter1 : if I want to filter sipm hits using the sipm position and dimension from GEANT4 simulation
    """
    sipm_position_info from Geant4 Simulation
    possible hit x , y values =  {(-201, -200), (200, 201), (-156, -155), (-118.5, -117.5), (-82, -81), (-45, -44), (44, 45),
                                   (81, 82), (117.5, 118.5), (155, 156)}
    possible hit z values = {(-0.5 + 1.25, 0.5 + 1.25), (10 + 2 * 0.2019 + 0.5 + 1.25, 10 + 2 * 0.2019 - 0.5 + 1.25), 
                          (20 + 4 * 0.2019 + 0.5 + 1.25, 20 + 4 * 0.2019 - 0.5 + 1.25), 
                          (30 + 6 * 0.2019 + 0.5 + 1.25, 30 + 6 * 0.2019 - 0.5 + 1.25),
                          (5 + 1 * 0.2019 + 0.5 + 1.25, 5 + 1 * 0.2019 - 0.5 + 1.25), 
                          (15 + 3 * 0.2019 + 0.5 + 1.25, 15 + 3 * 0.2019 - 0.5 + 1.25),
                          (25 + 5 * 0.2019 + 0.5 + 1.25, 25 + 5 * 0.2019 - 0.5 + 1.25), 
                          (35 + 7 * 0.2019 + 0.5 + 1.25, 35 + 7 * 0.2019 - 0.5 + 1.25)}
    """
    print("hits:", hits)
    #allowed_x_y_range = [(-201, -200), (200, 201), (-156, -155), (-118.5, -117.5), (-82, -81), (-45, -44), (44, 45),
                                  # (81, 82), (117.5, 118.5), (155, 156)]
    allowed_x_y_range = [(-201-0.5, -200+0.5), (200-0.5, 201+0.5), (-156-0.5, -155+0.5), 
                         (-118.5-0.5, -117.5+0.5), (-82-0.5, -81+0.5), (-45-0.5, -44+0.5), (44-0.5, 45+0.5),
                                   (81-0.5, 82+0.5), (117.5-0.5, 118.5+0.5), (155-0.5, 156+0.5)]
    allowed_z_range = [(-2 * 0.5 + 1.25, 2 * 0.5 + 1.25), (10 + 2 * 0.2019 - 2 * 0.5 + 1.25, 10 + 2 * 0.2019 + 2 * 0.5 + 1.25), 
                          (20 + 4 * 0.2019 - 2 * 0.5 + 1.25, 20 + 4 * 0.2019 + 2 * 0.5 + 1.25), 
                          (30 + 6 * 0.2019 - 2 * 0.5 + 1.25, 30 + 6 * 0.2019 + 2 * 0.5 + 1.25),
                          (5 + 1 * 0.2019 - 2 * 0.5 + 1.25, 5 + 1 * 0.2019 + 2 * 0.5 + 1.25), 
                          (15 + 3 * 0.2019 - 2 * 0.5 + 1.25, 15 + 3 * 0.2019 + 2 * 0.5 + 1.25),
                          (25 + 5 * 0.2019 - 2 * 0.5 + 1.25, 25 + 5 * 0.2019 + 2 * 0.5 + 1.25), 
                          (35 + 7 * 0.2019 - 2 * 0.5 + 1.25, 35 + 7 * 0.2019 + 2 * 0.5 + 1.25)]
    
    def build_mask(series, x_y_range):
        mask = np.zeros(len(series), dtype=bool)
        for low, high in x_y_range:
            mask |= (series >= low) & (series <= high)
        return mask

    x_mask = build_mask(hits['X(mm)'], allowed_x_y_range)
    y_mask = build_mask(hits['Y(mm)'], allowed_x_y_range)
    
    
    filtered_hits = hits[x_mask & y_mask]
    print('filtered_hits:', filtered_hits)

    print("before:", len(hits)) # 12133975
    print("after:", len(filtered_hits)) # 805144
    #z_mask = build_mask(filtered_hits['Z(mm)'], allowed_z_range)
    #final_filtered_hits = filtered_hits[z_mask]
    #print('final filtered_hits:', final_filtered_hits)

    print("before any filter:", len(hits)) # 12133975
    #print("after x,y,z filter:", len(final_filtered_hits)) # 805144       No need for z filter
    
    #hits = filtered_hits # To use filter 1, uncomment this line
    
    
    event_count = hits.groupby(['RunID', 'EventID']).size().reset_index(name='hit_count')
    #print("event_count:", event_count)
    repeating_events = event_count[event_count['hit_count'] > 1]
    #print("repeating_events:", repeating_events)
    hit_count_max = repeating_events['hit_count'].max()
    #print("hit_count_max:", hit_count_max) # 107
    
    # filter 2 
    # checking multiple annihil event exist with same RunID and EventID, eliminating such duplicates
    ann_count = ann.groupby(['RunID', 'EventID']).size().reset_index(name='hit_count')
    #print("ann_count:", ann_count)
    repeating_ann_events = ann_count[ann_count['hit_count'] > 1]
    #print("repeating_ann_events:", repeating_ann_events)
    if not repeating_ann_events.empty:
        ann = ann.drop_duplicates(subset=['RunID', 'EventID'], keep='first')
        #print('ann:', ann)
        print("annihilation duplicates are removed")
    else:
        print("No annihilation duplicates exist")
    
    # filter 3
    # checking multiple neucapture event exist with same RunID and EventID, eliminating such duplicates
    neu_count = neu.groupby(['RunID', 'EventID']).size().reset_index(name='hit_count')
    #print("neu_count:", neu_count)
    repeating_neu_events = neu_count[neu_count['hit_count'] > 1]
    #print("repeating_neu_events:", repeating_neu_events)
    
    if not repeating_neu_events.empty:
        neu = neu.drop_duplicates(subset=['RunID', 'EventID'], keep='first')
        #print('neu:', neu)
        print("neutroncapture duplicates are removed")
    else:
        print("No neutroncapture duplicates found")

    
    return hit_count_max, ann, neu, hits

def analysis(df):
    # annihilation and neutroncapture info from sipm hit combined data
    # trying to understand more about photon hits on sipms from annihilation events and neutroncapture events
    ann_hits = df[df['class'] == 1]
    neu_hits = df[df['class'] == 0]
    

    print('ann_hits:', ann_hits)
    print('neu_hits:', neu_hits)
    ann_hits_time = ann_hits[['tt', 'Time(ns)']]
    neu_hits_time = neu_hits[['tt', 'Time(ns)']]
    
    print('ann time min:', ann_hits_time['tt'].min()) # 1.99337e-05
    print('ann time max:', ann_hits_time['tt'].max()) # 25.7945
    print('ann sipm hit time min:', ann_hits_time['Time(ns)'].min()) # 1.34636
    print('ann sipm hit time max:', ann_hits_time['Time(ns)'].max()) # 32.5092
    print('neu time min:', neu_hits_time['tt'].min()) #  0.7982
    print('neu time max:', neu_hits_time['tt'].max()) # 270643.0
    print('neu sipm hits time min:', neu_hits_time['Time(ns)'].min()) # 2.21124
    print('neu sipm hits time max:', neu_hits_time['Time(ns)'].max()) # 270647.0

    #print(df[df['Time(ns)'] == neu_hits_time['Time(ns)'].max()])

    

def preprocess_data(hits, ann, neu, max_hits=120):#max_hits=600):
    '''     
    We don't have sipm hits coming soleley from annihil and neucapture events
    we have sipm hits coming from all kinds of interactions including annihil and neucapture events
    identifying sipm hits coming soleley from annihil and neucapture events is important for training
    To do this, first compare same runid and eventid exist in both sipm hits and ann/neu event,
    which indicates same electron/neutron produced sipm hits and also caused annihil/neucapture, but doesn't guarantee sipm hits
    are coming from same annihil/neucapture events, 
    example sipm hits can be from cherenkov photons from electron
    To filter sipm hits coming soleley from annihil and neucapture events, 
    consider only hits with time difference between annihil/neucapture event and sipm hit from annihil/neucapture event is 10 ns
    '''
    print('ann:', ann)
    print('neu:', neu)
    
    # Comparison of sipm hits and annihilation events
    # identifying matching runid and eventid from both sipmhits and annihil events
    hits_ann_common = pd.merge(hits, ann, on=['RunID', 'EventID'])
    print('hits_ann_common:', hits_ann_common) # 353554  5317859
    # adding  new features  1) distance between Sipm hit position and annihi event 2) time difference between sipm hit and annihi event
    hits_ann_common['Distance (mm)'] = np.sqrt((hits_ann_common['X(mm)'] - hits_ann_common['tx'])**2 
                                            + (hits_ann_common['Y(mm)'] - hits_ann_common['ty'])**2
                                            + (hits_ann_common['Z(mm)'] - hits_ann_common['tz'])**2)
                                            
    hits_ann_common['dt'] = hits_ann_common['Time(ns)'] - hits_ann_common['tt'] # Time Diff must be positive
    print('hits_ann_common:', hits_ann_common)
    # Time Diff must be positive, because annihilation event happens first, sipmhits happen later
    hits_ann_common_positive_time_diff = hits_ann_common[hits_ann_common['dt'] > 0] 
    print('hits_ann_common_positive_time_diff :', hits_ann_common_positive_time_diff) #353550
    hits_ann_common = hits_ann_common_positive_time_diff

    # assume time difference between annihil event and sipm hit from annihil event is 10 ns
    # we can change this 10 ns value, based on model performance
    hit_mask = hits_ann_common['Time(ns)'] < (hits_ann_common['tt'] + 10)
    hits_prompt_only = hits_ann_common[hit_mask].copy()
    print(f"Total hits before: {len(hits_ann_common)}")
    # These are true sipm hits from annihilation
    print('hits_prompt_only:', hits_prompt_only) 
    print(f"Total prompt hits after: {len(hits_prompt_only)}")
    hits_ann_common = hits_prompt_only
    hits_ann_common['class'] = 1 # annihilation event
   
    '''
    # plot hist of distance between Sipm hit position and ann event
    fig = plt.figure(figsize=(8,6))
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter3D(hits_ann_common['X(mm)'], hits_ann_common['Y(mm)'], 
                                       hits_ann_common['Z(mm)'], color='red', marker='o', alpha = 0.3, 
                                       label='sipm hit')
    ax.scatter3D(hits_ann_common['tx'], hits_ann_common['ty'], 
                                       hits_ann_common['tz'], color='green', marker='^', alpha = 0.3, 
                                       label ='annihilation event')
    ax.set_xlabel('X Axis')
    ax.set_ylabel('Y Axis')
    ax.set_zlabel('Z Axis')
    ax.set_title('Sipm hit vs Annihilation event')
    #plt.legend()
    plt.savefig('SipmhitvsAnnihilationevent.png')
    plt.show()
    '''
    # Comparison of sipm hits and neutroncapture events
    hits_neu_common = pd.merge(hits, neu, on=['RunID', 'EventID'])
    print('hits_neu_common:', hits_neu_common) # 9241  140167
    # adding  new features  1) distance between Sipm hit position and neu event 2) time difference between sipm hit and neu event
    hits_neu_common['Distance (mm)'] = np.sqrt((hits_neu_common['X(mm)'] - hits_neu_common['tx'])**2 
                                            + (hits_neu_common['Y(mm)'] - hits_neu_common['ty'])**2
                                            + (hits_neu_common['Z(mm)'] - hits_neu_common['tz'])**2)
                                            
    hits_neu_common['dt'] = hits_neu_common['Time(ns)'] - hits_neu_common['tt'] # Time Diff must be positive
    print('hits_neu_common:', hits_neu_common)
    hits_neu_common_positive_time_diff = hits_neu_common[hits_neu_common['dt'] > 0]
    print('hits_neu_common_positive_time_diff :', hits_neu_common_positive_time_diff) # 258
    
    hits_neu_common = hits_neu_common_positive_time_diff  # only consider positive time diff events
    hit_mask1 = hits_neu_common['Time(ns)'] < (hits_neu_common['tt'] + 10)
    hits_prompt_only1 = hits_neu_common[hit_mask1].copy()
    print(f"Total hits before: {len(hits_neu_common)}")
    print('hits_prompt_only1:', hits_prompt_only1)
    print(f"Total prompt hits after: {len(hits_prompt_only1)}")
    hits_neu_common = hits_prompt_only1
    hits_neu_common['class'] = 0
    
    '''
    # plot hist of distance between Sipm hit position and neu event
    fig = plt.figure(figsize=(8,6))
    ax = fig.add_subplot(111, projection='3d')
    ax.scatter3D(hits_neu_common['X(mm)'], hits_neu_common['Y(mm)'], 
                                       hits_neu_common['Z(mm)'], color='red', marker='o', alpha = 0.3,
                                        label='sipm hit')
    ax.scatter3D(hits_neu_common['tx'], hits_neu_common['ty'], 
                                       hits_neu_common['tz'], color='green', marker='^', alpha = 0.3, 
                                       label ='neutron capture event')
    ax.set_xlabel('X Axis')
    ax.set_ylabel('Y Axis')
    ax.set_zlabel('Z Axis')
    ax.set_title('Sipm hit vs Neutron capture event')
    plt.legend()
    plt.savefig('SipmhitvsNeutroncaptureevent.png')
    plt.show()
    '''

    df = pd.concat([hits_ann_common, hits_neu_common])
    df['log(time)'] = np.log10(df['Time(ns)'] + 1)
    df = df.drop(['Distance (mm)'], axis=1) # removing distance column, not necessary for learning
    print("df:", df.head(20))
    
    analysis(df)
    
    X_list = []
    Y_list = []
    
    grouped = df.groupby(['RunID', 'EventID'])
    for name, group in grouped:
        group = group.sort_values("dt")   
        event_hits = group[['X(mm)', 'Y(mm)', 'Z(mm)', 'dt', 'log(time)']].values.astype(np.float32)
        #event_hits = group[['X(mm)', 'Y(mm)', 'Z(mm)', 'dt', 'log(time)']].values
        
        # truncate the hits from an event
        if len(event_hits) > max_hits:
            event_hits = event_hits[:max_hits]
        
        #padded_hits = np.zeros((max_hits, 5)) 
        padded_hits = np.full((max_hits, 5), np.nan)
        #padded_hits = np.full((max_hits, 4), np.nan) 
        padded_hits[:len(event_hits), :] = event_hits
        
        X_list.append(padded_hits)
        
        # target: [tx, ty, tz, class]
        first_row = group.iloc[0]
        tt_val = first_row['tt']
        log_tt = np.log10(tt_val + 1) # prevents math errors if tt is 0
        Y_list.append([first_row['tx'], first_row['ty'], first_row['tz'], log_tt, first_row['class']])

        #print("event_hits:", event_hits)

    return np.array(X_list), np.array(Y_list)

def build_model(max_hits=120, n_features=5, padding_value=0.0):
#def build_model(max_hits=120, n_features=5, padding_value=-9.9999e4):
#def build_model(max_hits=120, n_features=4, padding_value=-9.9999e4):
    inputs = layers.Input(shape=(max_hits, n_features))
    x = layers.Masking(mask_value=padding_value)(inputs)

    x = layers.Dense(64, activation='relu')(x)
    x = layers.Dense(64, activation='relu')(x)

    x = layers.GlobalAveragePooling1D()(x)

    # event-level features
    x = layers.Dense(128, activation='relu')(x)
    x = layers.Dropout(0.2)(x)

    reg_output = layers.Dense(4, activation='linear', name='reg_output')(x)
    class_output = layers.Dense(1, activation='sigmoid', name='class_output')(x)

    model = models.Model(inputs=inputs, outputs=[reg_output, class_output])
    model.compile(
        optimizer='adam',
        loss={
            'reg_output': 'mse',
            'class_output': 'binary_crossentropy'
        },
        loss_weights={
            'reg_output': 1.0,
            'class_output': 0.5
        },
        metrics={
            'reg_output': ['mae'],
            'class_output': ['accuracy']
        }
    )
    return model


def main():
    if len(sys.argv) < 3:
        print("Usage: python train_dnn.py sipmHits_output.csv annihilationEvents_output.csv neutronCaptureEvents_output.csv")
        sys.exit(1)

    print("Loading data.........................................................")
    hits_raw = load_hits_raw(sys.argv[1])
    ann = load_labels_one(sys.argv[2])
    neu = load_labels_one(sys.argv[3])
    hits_raw = hits_raw.rename(columns={"FileIndex" : "RunID"})
    ann = ann.rename(columns={"FileIndex" : "RunID"})
    neu = neu.rename(columns={"FileIndex" : "RunID"})
    print('hits:', hits_raw.head(10))
    print('ann:', ann.head(10))
    print('neu:', neu.head(10))
    print("files opened")
    
    hits_raw = hits_raw.sort_values(by=['RunID', 'EventID', 'Time(ns)'], ascending=True)
    ann = ann.sort_values(by=['RunID', 'EventID', 'tt'], ascending=True)
    neu = neu.sort_values(by=['RunID', 'EventID', 'tt'], ascending=True)

    # analyze data
    max_hits, ann, neu, hits_raw = hits_info (hits_raw, ann, neu)

    #print(preprocess_data(hits_raw, ann, neu, max_hits=100))
    X, Y = preprocess_data(hits_raw, ann, neu, max_hits)
    
    print(f"final input Shape: {X.shape}") 
    print(f"final target Shape: {Y.shape}") 
    print("X:", X)
    print("Y:", Y)

    X_train_full, X_test, y_train_full, y_test = train_test_split(X,Y, test_size=0.2)

    X_train, X_val, y_train, y_val = train_test_split(X_train_full, y_train_full, test_size=0.2, random_state=42)

    print("X_test:", X_test)
    print("y_test:", y_test)
    print("X_val:", X_val)
    print("y_val:", y_val)
    
    np.save("X_test_raw.npy", X_test)
    np.save("y_test_raw.npy", y_test)
    print("Test data saved for verification.")

    # replace all nan values to -99999
    #X_train = np.nan_to_num(X_train, nan=-99999)
    #X_test = np.nan_to_num(X_test, nan=-99999)
    #X_val = np.nan_to_num(X_val, nan=-99999)
    
    # replace all nan values to 0.0
    X_train = np.nan_to_num(X_train, nan=0.0)
    X_test = np.nan_to_num(X_test, nan=0.0)
    X_val = np.nan_to_num(X_val, nan=0.0)

    scaler_x = MinMaxScaler(feature_range=(-1,1))
    # flatten X_train to 2D to fit the scaler
    #X_train_2d = X_train.reshape(-1,4)
    X_train_2d = X_train.reshape(-1,5)
    print('X_train', X_train)
    print('X_train_2d', X_train_2d)
    
    #padding_value = -9.9999e4
    padding_value = 0.0
    masked_X_train = X_train_2d[:, 0] != padding_value
    print('masked_X_train', masked_X_train)
    # fit only on training data
    scaler_x.fit(X_train_2d[masked_X_train])

    def scale(data, scaler):
        data_2d = data.reshape(-1, 5)
        #data_2d = data.reshape(-1, 4)
        mask = data_2d[:, 0] != padding_value
        # Transform ONLY the real hits
        data_2d[mask] = scaler.transform(data_2d[mask])
        return data_2d.reshape(data.shape)

    # transform all sets using the train-fitted scaler
    X_train_scaled = scale(X_train, scaler_x)
    X_val_scaled = scale(X_val, scaler_x)
    X_test_scaled = scale(X_test, scaler_x)

    scaler_y = MinMaxScaler(feature_range=(-1, 1))

    # fit and transform coordinates only (first 3 columns: tx, ty, tz, log_tt)
    y_train_vtx = scaler_y.fit_transform(y_train[:, :4])
    y_val_vtx = scaler_y.transform(y_val[:, :4])   
    y_test_vtx = scaler_y.transform(y_test[:, :4]) 

    # keep labels (class) as they are (0 and 1)
    y_train_cls = y_train[:, 4]
    y_val_cls = y_val[:, 4]
    y_test_cls = y_test[:, 4]

    print('y_test_cls:', y_test_cls)
    
    model = build_model(max_hits=max_hits)
    model.summary()

    early_stop = tf.keras.callbacks.EarlyStopping(monitor='val_loss', patience=10, restore_best_weights=True)

    history = model.fit(X_train_scaled, 
                        [y_train_vtx, y_train_cls], 
                        validation_data=(X_val_scaled, [y_val_vtx, y_val_cls]),
                        epochs=100,
                        batch_size=64,
                        callbacks=[early_stop]
                        )
    model.save('reconstruction_model.keras')
    
    preds_vtx, preds_cls = model.predict(X_test_scaled)
    y_test_unscaled = scaler_y.inverse_transform(preds_vtx)
    predicted_tt = 10**(y_test_unscaled[:, 3]) - 1
    print(f"Sample Prediction (X, Y, Z): {y_test_unscaled[0, :3]}")
    print(f"Predicted Class (Prob): {preds_cls[0]}")

    joblib.dump(scaler_x, "scaler_x.joblib")
    joblib.dump(scaler_y, "scaler_y.joblib")

    print("all files saved successfully")

if __name__ == "__main__":
    main()