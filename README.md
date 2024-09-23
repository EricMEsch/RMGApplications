# remage applications

This repository is a collection of remage applications that are either meant for small tests or in early development until they might be moved somewhere more official.



<br/>

remage: https://github.com/legend-exp/remage


### Applications

* 01-FullCosmogenics is a starting point for cosmogenic simulations studying the production of <sup>77</sup>Ge in LEGEND-1000. Currently the geometry is an updated version of Warwick-Legend using an optical water tank with photomultipliers as optical detectors.
* 02-TestIsotopeFilter is mostly a copy from the ReMaGe examples with slightly modified construction and different macros. The purpose is to test the IsotopeFilter feature.
* 03-TestNeutronCapture employs a custom OutputScheme to output all Isotopes created via the nCapture process, to test the custom RMGNeutronCaptureProcess

### Running the applications
1. Create a build directory and compile the project:

    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```

2. Run the compiled executable while having the folder as working directory:

    ```sh
    cd ..
    ./build/example
    ```
