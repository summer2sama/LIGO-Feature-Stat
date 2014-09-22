LIGO-Feature-Stat
=================

Introduction
============
- LIGO-Feature-Stat is an application to measure the importance in classifying of features from LIGO detecters.
- The idea to measure the importance of features here is as follows. For a given testing point, we find the nearest point to it on the separating surface. Then we compute the margin on each feature between these two points and let the feature with biggest margin be the most important classifying feature for this individual testing point. Then for a set of many testing points, we could measure the importance of a  feature by the times of that be the most important classifying feature for a individual testing point. Then we could get a histogram for all the features and find something.

Algorithm
=========
1. Use the S6_100ms_full_train.dat.model to calculate the y value (y = w.psi(x) + b) of each Support Vector in the model.Set a threshold y_th and let the Support Vectors whose y value is smaller than y_th be the Hyper-Plane Vectors. 

2. For an individual testing Vector xi, which has been scaled, calculate the Euclidean distance between xi and every Hyper-Plane Vector. Found the nearest Hyper-Plane Vector hi with the smallest Euclidean distance.

3. Caculate the difference between xi and hi on each feature. Let the feature with the biggest (absolute) difference be the most discriminative feature for xi.

4. For dataset S6_100ms_set_0_train.dat.scale, go through samples as testing vector( for the result in github, I went throung 5000 samples). Get the number of times of each feature as the most discriminative feature to a testing sample.

Dataset
=======
- Get S6_100ms_full_train.dat.model to find the Hyper-Plane Vectors.
 - Information about the model is as follows
   +--------------+--------------+
   |svm_type      |c_svc         |
   +--------------+--------------+
   |kernel_type   |rbf           |
   +--------------+--------------+
   |gamma         |0.0078125     |
   +--------------+--------------+
   |nr_class      |2             |
   +--------------+--------------+
   |total_sv      |5211          |
   +--------------+--------------+
   |rho           |1.48704       |
   +--------------+--------------+
   |label         |1 -1          |
   +--------------+--------------+
   |probA         |-4.79841      |
   +--------------+--------------+
   |probB         |-0.89896      |
   +--------------+--------------+
   |nr_sv         |1980 3231     |
   +--------------+--------------+

- Get S6_100ms_set_0_train.dat.scale as testing dataset. For the Output showed below, we use 5000 samples of the dataset.


Compile and Run
===============
- Compile: $ g++ LFS.c svm.cpp -o LFS
- Run: $ ./LFS S6_100ms_full_train.dat.model S6_100ms_set_0_train.dat.scale

Output
======
- We could present some of the most important features here（the number of tesing set here is 5000):

  +---------------+---------------+
  |    index      | Feature_Count |
  +---------------+---------------+
  |     824       |     414       |
  +---------------+---------------+
  |     724       |     405       |
  +---------------+---------------+
  |     559       |     394       |
  +---------------+---------------+
  |     709       |     346       |
  +---------------+---------------+
  |     644       |     222       |
  +---------------+---------------+
  |     774       |     196       |
  +---------------+---------------+
  |     642       |     171       |
  +---------------+---------------+
  |     584       |     157       |
  +---------------+---------------+
  |     579       |     138       |
  +---------------+---------------+
  |     934       |     124       |
  +---------------+---------------+

- Obviously, The most important features are from the 4th feature of detecters.
- For most features, their count is zero. From the histogram, We could see a few pulses on it.
