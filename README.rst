LIGO-Feature-Stat
=================

Introduction
============
- LIGO-Feature-Stat is an application to measure the importance in classifying of features from LIGO detecters.
- The idea to measure the importance of features here is as follows. For a given testing point, we find the nearest point to it on the separating surface. Then we compute the margin on each feature between these two points and let the feature with biggest margin be the most important classifying feature for this individual testing point. Then for a set of many testing points, we could measure the importance of a  feature by the times of that be the most important classifying feature for a individual testing point. Then we could get a histogram for all the features and find something.


Compile and Run
===============
- Get S6_100ms_set_0_train.dat.scale as testing dataset.
- Get S6_100ms_full_train.dat.model to find the points on the separating suiface.
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
