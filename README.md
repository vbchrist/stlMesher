# stlMesher

Usage: stlRefine [OPTIONS] input_file.stl output_file.stl

Options:
  -h [ --help ]         Display help menu.
  -v [ --version ]      Display program version number
  -l [ --length ] arg   Mesh target edge length.
  -a [ --angle ] arg    Minimum dihedral angle between facets sharing used for
                        edge detection.
  --input-files arg     Requires two files input_file.stl output_file.stl