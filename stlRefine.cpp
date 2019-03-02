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
#include "STL_writer.h"

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Surface_mesh<Kernel::Point_3> Mesh;

int main(int argc, char *argv[])
{
  std::ifstream input(argv[1], std::ios::binary);
  std::vector<CGAL::cpp11::array<double, 3>> points;
  std::vector<CGAL::cpp11::array<int, 3>> triangles;

  CGAL::read_STL(input, points, triangles);

  std::cout << "Points: " << points.size() << "\n";
  std::cout << "Triangles: " << triangles.size() << "\n";

  Mesh m;

  if (CGAL::Polygon_mesh_processing::is_polygon_soup_a_polygon_mesh(triangles))
  {
    std::cout << "it is a polygon mesh\n";
    CGAL::Polygon_mesh_processing::polygon_soup_to_polygon_mesh(points, triangles, m);
    if (!m.is_valid() || m.is_empty())
    {
      std::cerr << "Error: Invalid polyhedron\n";
      return 0;
    }

    double target_edge_length = atof(argv[3]);
    unsigned int nb_iter = 20;
    boost::property_map<Mesh, CGAL::edge_is_feature_t>::type eif = get(CGAL::edge_is_feature, m);
    double angle = 5;

    CGAL::Polygon_mesh_processing::detect_sharp_edges(
        m,
        angle,
        eif);

    CGAL::Polygon_mesh_processing::isotropic_remeshing(
        faces(m),
        target_edge_length,
        m,
        CGAL::Polygon_mesh_processing::parameters::number_of_iterations(nb_iter)
            .edge_is_constrained_map(eif)
            );

    std::cout << "Remeshing done.\n";

    std::ofstream outfile(argv[2]);

    CGAL::write_STL(m, outfile);

    outfile.close();
  }

  return 0;
}