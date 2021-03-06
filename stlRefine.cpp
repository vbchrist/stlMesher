// std
#include <iostream>
#include <fstream>
#include <string>

// CGAL
#include <CGAL/IO/STL_reader.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/Polygon_mesh_processing/detect_features.h>
#include <CGAL/Polygon_mesh_processing/remesh.h>

#include <CGAL/Polygon_mesh_processing/transform.h>
#include <CGAL/Polygon_mesh_processing/bbox.h>

// local
#include "program_input.h"
#include "STL_writer.h"

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3> Mesh;

int main(int argc, char *argv[])
{
  prog_inputs prog_args(argc, argv);

  if (prog_args.is_err())
  {
    return 0;
  };

  std::cout << "\nReading mesh from " << prog_args.get_input_file() << "\n";
  std::ifstream input(prog_args.get_input_file(), std::ios::binary);
  std::vector<CGAL::cpp11::array<double, 3>> points;
  std::vector<CGAL::cpp11::array<int, 3>> triangles;

  CGAL::read_STL(input, points, triangles);
  if (CGAL::Polygon_mesh_processing::is_polygon_soup_a_polygon_mesh(triangles))
  {
    Mesh m;
    double surface_area_orig = 0., volume_orig = 0.,  surface_area_new = 0.,  volume_new = 0.;

    CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, triangles, m);
    if (!m.is_valid() || m.is_empty())
    {
      std::cout << "\nError: Mesh was read from file but is not valid or is empty.\n\n";
      std::cout << "Cannot proceed. Exiting Program.\n";
      return 0;
    }
    else
    {
      surface_area_orig = CGAL::Polygon_mesh_processing::area(m);
      volume_orig = CGAL::Polygon_mesh_processing::volume(m);

      std::cout << "Surface mesh sucessfully read.\n";
      std::cout << "  Points: " << points.size() << "\n";
      std::cout << "  Triangles: " << triangles.size() << "\n";
      std::cout << "  Surface area: " << surface_area_orig << "\n";
      if (CGAL::is_closed(m))
      {
        std::cout << "  Mesh is closed and bounds a volume.\n";
        std::cout << "  Bound volume: " << volume_orig << "\n";
        std::cout << "  Bounding box [" << CGAL::Polygon_mesh_processing::bbox(m) << "]\n";
      }
      else
      {
        std::cout << "  Mesh is not closed.\n";
      }
    }


    //CGAL::Polygon_mesh_processing::transform(Kernel::Aff_transformation_3(CGAL::TRANSLATION, Kernel::Vector_3(1000,0,0)), m);
    if(prog_args.get_scale() != 1.){
      std::cout << "\nScaling mesh by a factor of " << prog_args.get_scale() << "\n";
      CGAL::Polygon_mesh_processing::transform(Kernel::Aff_transformation_3(CGAL::SCALING, prog_args.get_scale()), m);   
    } 
 

    double target_edge_length = prog_args.get_edge_length();
    double angle_in_deg = prog_args.get_dihedral_angle();
    unsigned int nb_iter = 20;
    boost::property_map<Mesh, CGAL::edge_is_feature_t>::type eif = get(CGAL::edge_is_feature, m);

    CGAL::Polygon_mesh_processing::detect_sharp_edges(
        m,
        angle_in_deg,
        eif
        );

    std::cout << "\nRemeshing with a target dihedral angle of " << angle_in_deg << " degrees and edge length of " << target_edge_length << "\n";

    CGAL::Polygon_mesh_processing::isotropic_remeshing(
        faces(m),
        target_edge_length,
        m,
        CGAL::Polygon_mesh_processing::parameters::number_of_iterations(nb_iter)
            .protect_constraints(true)
            .edge_is_constrained_map(eif)
        );

    std::cout << "Remeshing complete.\n";
   
    surface_area_new = CGAL::Polygon_mesh_processing::area(m);
    volume_new = CGAL::Polygon_mesh_processing::volume(m);
    double area_err_pc = (surface_area_new/pow(prog_args.get_scale(),2.) - surface_area_orig)/surface_area_orig*100;
    double volume_err_pc = (volume_new/pow(prog_args.get_scale(),3.) - volume_orig)/volume_orig*100;

    std::cout << "  Surface area: " << surface_area_new << "    Error " << area_err_pc << "% \n";
    if (CGAL::is_closed(m))
    {
      std::cout << "  Bound volume: " << volume_new << "    Error " << volume_err_pc << "% \n";
      std::cout << "  Bounding box [" << CGAL::Polygon_mesh_processing::bbox(m) << "]\n";
    }

    std::cout << "\nWriting new mesh to " << prog_args.get_output_file() << "\n";
    std::ofstream outfile(prog_args.get_output_file());
    CGAL::write_STL(m, outfile);
    outfile.close();
  }
  else
  {
    std::cout << "\nError: No valid mesh was read from file.\n\n";
    std::cout << "Cannot proceed. Exiting Program.\n";
  }
  return 0;
}