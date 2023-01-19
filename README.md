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

Second, the library Research-M is organized by submodule in git and you should ``git submodule update --progress --init --force `` to download all of the submodule.

### 2.1 Use Research-M in Your Project

Download Research-M, open folder ``ResearchM`` and you will find a file ``ResearchM.cmake``.  Then

1. In the ``CmakeLists.txt``  of your project, in include the ``ResearchM.cmake``, for example ``include(./Research-M/ResearchM/ResearchM.cmake)``

2. Include root path of the header files to your project: ``include_directories(${ResearchM_INCLUDE_DIRS})``

3. Call the environment setup function in ``ResearchM.cmake``: ``Set_ResearchM_Env()``

4. Add dependency of the library to the project:  ``ResearchV_Deps(${project_name})``

Note that Step 1. 2. 3.4. should be done before you ``add_executable``.

Now you could use the library Research-M in your project!

### 2.2 Use the examples of Research-M

You only need to build all of the projects in examples folder of Research-M by ``cmake`` the ``CmakeLists.txt`` in the root path of ``Research-M``.

You could use ``cmake-gui``, ``cmake`` command or others, and you also could use the script ``make_project.py`` in Research-M.  ``make_project.py`` will first git update submodule and then build all   example projects.  You only need:

```sh
python make_project.py
```

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

### 1. Purpose of Research-G

During my internship of physical simulation in Dartmouth, we use a code library called simplex and develop another code library called Meso.  I find that both of these libraries are too  dependent on the  features of the mathematical calculation library they use, Eigen.   Eigen sometimes can not be used in GPU and therefore it is not easy to make codes in Meso and Simplex to run on GPU easily.  Also, Meso and Simplex do not provides a uniformly tool to visualize the results of the physical simulation and for every experiments, I need to design visualization tool for my purpose.

I consider whether I can construct a uniform code library for the purpose of research on computer graphics, which have uniformly-designed tools from mathematical computation to physical simulation and visualization and researchers in computer graphics could do researches easily.  For this reason, Research-G is born.











