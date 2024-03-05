from scipy.spatial.transform import Rotation   
# This script is used as a guess for the rotation matrix of the front camera based on the IMU unit of an imagry trip.
# Place the following 3x3 matrix in the data 4x4 matrix within the LeftCameraParams.yaml file.
Rotation.from_euler('x', -70, degrees=True).as_matrix()

# Matrix of Euroc dataset:
# R = [[0.014, -0.99, 0.004], [0.999, 0.014, 0.025], [-0.025, 0.0037, 0.999]]
# r =  Rotation.from_matrix(R)
# angles = r.as_euler("zyx",degrees=True)
# angles