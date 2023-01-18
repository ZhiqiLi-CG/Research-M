# Research-M

**Substitution for Eigen but not only Eigen!**

But please note that the purpose of this code library is for computer graphics and visualization(including geometry, physical simulation, topology, cluster, TDA, ....) and their related scientific computing.  Our purpose is different from GLS, Eigen, Matlab, ...  Refer to Appendix for the purpose.

## 1. Introduction

You may ask, "Why Research-M?", since there are so many computation library like Eigen.  However, Eigen have the following disadvantages:

- Lazy Evaluation of Eigen is unstable and it is easy to be misused, especially in kernel function
- Only basic functions of Eigen could be used in device directly and many complexed functions could not be used, like computing eigen vectors of several small matrix. (Note: This senario exists.  Though cublas can compute a large matrix parallel, cublas could not compute several small matrix easily. )  

Research-M solves these problems and it also have other advantages:

- Research-M provides other advanced function, like homology computation of topology, moving linear square fitting and other geometry calculation.  
- It also provides other easy-to-use functions.  
- The functions in Research-M are designed to be used in both host and device, which means that you could write complexed kernel functions in CUDA based on the tools provided by Research-M.
- All files of Research-M is header file, which make it easy to use.  You only need to download the library and use only 2 functions in ``ResearchM.cmake`` to help you add this library to your project.   

## 2. How to use it?

First, you should ensure that you have install Visual Studio 2022 and Research-M only can be used in windows.

### 2.1 Use Research-M in Your Project

### 2.2 Use the examples of Research-M

## 3. Basic Structure

 This library only have one developer.  Therefore the the basic structure of Research-M is kind of dynamic based on some fixed design.  The update of the basic structure will be recorded here

- ~2023/1/19

  1. namespace

     - Some basic codes, like the ``vector`` structure and ``matrix`` structure of Research-M is put directly in the namespace of ``zq``.   These codes are the basis of basis of Research-M

     - Some codes, which have little relation to math, is put in the namespace of ``zq::utils``

     - Some codes, which have strong relation to math, is put in the namespace of ``zq::math``

       TODO: the codes in ``zq::math`` will be divided into several part, like ``algorithm``, ``geometry``, ``topology``,.....

- ...



## Appendix

### 1. Purpose

During my internship of physical simulation in Dartmouth, we use a code library called simplex and develop another code library called Meso.  At that time, I always consider why physical simulation researchers take great interest in making own code library.  











