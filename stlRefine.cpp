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

  Mesh m;

  if (CGAL::Polygon_mesh_processing::is_polygon_soup_a_polygon_mesh(triangles))
  {
    CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, triangles, m);
    if (!m.is_valid() || m.is_empty())
    {
      std::cout << "\nError: Mesh was read from file but is not valid or is empty.\n\n";
      std::cout << "Cannot proceed. Exiting Program.\n";
      return 0;
    }
    else
    {
      std::cout << "Mesh sucessfully read.\n";
      std::cout << "  Points: " << points.size() << "\n";
      std::cout << "  Triangles: " << triangles.size() << "\n";
    }

    double target_edge_length = prog_args.get_edge_length();
    double angle_in_deg = prog_args.get_dihedral_angle();
    unsigned int nb_iter = 20;
    boost::property_map<Mesh, CGAL::edge_is_feature_t>::type eif = get(CGAL::edge_is_feature, m);

    CGAL::Polygon_mesh_processing::detect_sharp_edges(
        m,
        angle_in_deg,
        eif);

    std::cout << "\nRemeshing with a target dihedral angle of " << angle_in_deg << " degrees and edge length of " << target_edge_length << "\n";

    CGAL::Polygon_mesh_processing::isotropic_remeshing(
        faces(m),
        target_edge_length,
        m,
        CGAL::Polygon_mesh_processing::parameters::number_of_iterations(nb_iter)
            .protect_constraints(true)
            .edge_is_constrained_map(eif)
            );

    std::cout << "Remeshing somplete.\n";

    std::cout << "Writing new mesh to " << prog_args.get_output_file() << "\n";
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