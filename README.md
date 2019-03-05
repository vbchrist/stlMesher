# stlMesher

Installation:

Download source code.
git clone https://github.com/vbchrist/stlMesher.git

Install required packages:

Install cmake 3.10 or later.
sudo apt install cmake

Download CGAL-4.13 or later.
https://github.com/CGAL/cgal/releases

Extract and build CGAL
tar xf {path to download}/CGAL-4.13.tar.xz
cd CGAL-4.13
cmake .
make

More details https://doc.cgal.org/latest/Manual/installation.html

Edit makeAll.sh to reflect CGAL install directory.
Change this line "make -DCGAL_DIR={path to download}/CGAL-4.13 -DCMAKE_BUILD_TYPE=Debug .."

Run makeAll.sh to build program, output binary stlMesher is located in debug/ folder.
./makeAll

Copy binary to current directory 
cp debug/stlMesher .

Run test on Sphericon.stl
./stlMesher -l 100 Sphericon.stl testOutput.stl


Usage: stlRefine [OPTIONS] input_file.stl output_file.stl

Options:
  -h [ --help ]         Display help menu.
  -v [ --version ]      Display program version number
  -l [ --length ] arg   Mesh target edge length.
  -a [ --angle ] arg    Minimum dihedral angle between facets sharing used for
                        edge detection.
  --input-files arg     Requires two files input_file.stl output_file.stl
