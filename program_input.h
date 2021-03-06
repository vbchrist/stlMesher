
#include <string>
#include <iostream>

//Boost
#include "boost/program_options.hpp"

class prog_inputs
{
  public:
    prog_inputs(int argc, char *argv[])
    {
        namespace po = boost::program_options;
        po::options_description description("Usage: stlRefine [OPTIONS] input_file.stl output_file.stl\n\nOptions");
        description.add_options()
            ("angle,a", po::value<double>(), "Minimum dihedral angle between facets sharing used for edge detection.")
            ("help,h", "Display help menu.")
            ("length,l", po::value<double>(), "Mesh target edge length.")
            ("scale,s", po::value<double>(), "Mesh scale factor.")
            ("translate,t", po::value<double>(), "Translate mesh by vector. Format: -t (X Y Z)")
            ("version,v", "Display program version number")
            ("input-files", po::value<std::vector<std::string>>(), "Requires two files input_file.stl output_file.stl");

        po::positional_options_description p;
        p.add("input-files", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(description).positional(p).run(), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << description;
            input_error = true;
        }
        else
        {
            // Input/output files
            std::vector<std::string> files;
            if (vm.count("input-files"))
            {
                files = vm["input-files"].as<std::vector<std::string>>();
            }
            if(files.size() != 2){
                std::cout << "Input and output file names required.\n\nstlRefine [OPTIONS] input_file.stl output_file.stl\n\n";
                input_error = true;
            }else{
                input_file = files[0];
                output_file = files[1];
            }

            // Edge length
            if (vm.count("length"))
            { 
                edge_length = vm["length"].as<double>();
                std::cout << "Target edge length: " << edge_length << "\n";               
            }
            else
            {
                std::cout << "Target edge is a required input.  Please provide using flag -l.\n  See help (-h, --help) for details.\n";
                input_error = true;
            }

            // Dihedral angle
            if (vm.count("angle"))
            {
                dihedral_angle = vm["angle"].as<double>();
                std::cout << "Dihedral angle in degrees: " << dihedral_angle << "\n";
            }
            else
            {
                std::cout << "Dihedral angle in degrees: " << dihedral_angle << " <default>\n";
            }

            // Scale
            if (vm.count("scale"))
            {
                scale = vm["scale"].as<double>();
                std::cout << "Scale factor: " << scale << "\n";
            }
            else
            {
                std::cout << "Scale factor: " << scale << " <default>\n";
            }

            if (input_error == true)
            {
                std::cout << "Cannot proceed. Exiting Program.\n";
            }
        }
    };

    std::string get_input_file() { return input_file; };
    std::string get_output_file() { return output_file; };
    double get_edge_length() { return edge_length; };
    double get_dihedral_angle() { return dihedral_angle; };
    double get_scale() { return scale; };
    bool is_err() { return input_error; };

  private:
    bool input_error = false;
    double edge_length;
    double dihedral_angle = 5.;
    double scale = 1.;
    std::string input_file;
    std::string output_file;

};