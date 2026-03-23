# Usage: python predict_dnn.py
import numpy as np
import numpy as np
import joblib
import matplotlib.pyplot as plt
from tensorflow.keras.models import load_model
import keras
import tensorflow as tf

def main():
    # load the model and scalers
    #keras.config.enable_unsafe_deserialization()
    model = load_model('reconstruction_model.keras')
    scaler_x = joblib.load("scaler_x.joblib")
    scaler_y = joblib.load("scaler_y.joblib")
    #padding_value = -9.9999e4
    padding_value = 0.0

    # load the saved test data
    X_test_raw = np.load("X_test_raw.npy")
    y_test_raw = np.load("y_test_raw.npy") 
    
    print("X_test_raw:", X_test_raw)

    X_test_raw = np.nan_to_num(X_test_raw, nan=padding_value)
    # scale the test data 
    X_shape = X_test_raw.shape
    X_2d = X_test_raw.reshape(-1, 5)
    mask = X_2d[:, 0] != padding_value
    X_2d[mask] = scaler_x.transform(X_2d[mask])
    X_test_scaled = X_2d.reshape(X_shape)
    print("X_test_raw:", X_test_raw)
    # predict
    print("Predicting on test set...")
    preds_vtx, preds_cls = model.predict(X_test_scaled)
    

    # unscale Predictions
    y_reco = scaler_y.inverse_transform(preds_vtx)
    predicted_tt = 10**(y_reco[:, 3]) - 1

    print("y_reco:", y_reco)
    # compare with truth
    true_x, true_y, true_z = y_test_raw[:, 0], y_test_raw[:, 1], y_test_raw[:, 2]
    reco_x, reco_y, reco_z = y_reco[:, 0], y_reco[:, 1], y_reco[:, 2]

    # calculate Euclidean error (accuracy)
    error = np.sqrt((true_x - reco_x)**2 + (true_y - reco_y)**2 + (true_z - reco_z)**2)
    print(f"Average Vertex Error: {np.mean(error):.2f} mm")

    # plot comparison for true and predicted events
    fig = plt.figure(figsize=(10, 7))
    ax = fig.add_subplot(111, projection='3d')
    #ax.scatter(true_x[:100], true_y[:100], true_z[:100], c='blue', label='Truth', alpha=0.5)
    #ax.scatter(reco_x[:100], reco_y[:100], reco_z[:100], c='red', label='Prediction', alpha=0.5)
    ax.scatter(true_x, true_y, true_z, c='blue', label='Truth', alpha=0.5)
    ax.scatter(reco_x, reco_y, reco_z, c='red', label='Prediction', alpha=0.5)
    ax.set_title("Reconstruction: Truth (Blue) vs Prediction (Red)")
    plt.legend()
    plt.savefig('comparison.png')
    plt.show()


if __name__ == "__main__":
    main()

