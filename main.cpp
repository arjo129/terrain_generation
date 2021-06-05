#include <igl/opengl/glfw/Viewer.h>
#include <igl/copyleft/cgal/mesh_boolean.h>
#include <random>

struct Mesh
{
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
};

Mesh generateCube(
  double length,
  double width,
  double height
)
{
  Mesh mesh;
  mesh.V = (Eigen::MatrixXd(8,3)<<
    0.0,    0.0,    0.0,
    0.0,    0.0,    height,
    0.0,    width,  0.0,
    0.0,    width,  height,
    length, 0.0,    0.0,
    length, 0.0,    height,
    length, width,  0.0,
    length, width,  height).finished();
  mesh.F = (Eigen::MatrixXi(12,3)<<
    1,7,5,
    1,3,7,
    1,4,3,
    1,2,4,
    3,8,7,
    3,4,8,
    5,7,8,
    5,8,6,
    1,5,6,
    1,6,2,
    2,6,8,
    2,8,4).finished().array()-1;
  return mesh;
}

void translateMesh(Mesh& mesh, Eigen::Vector3d vec)
{
  mesh.V.rowwise() += vec.transpose();
}

Mesh combineMeshes(const Mesh& m1, const Mesh& m2)
{
  Mesh result;
  igl::copyleft::cgal::mesh_boolean(
    m1.V, m1.F,
    m2.V, m2.F,
    igl::MESH_BOOLEAN_TYPE_UNION,
    result.V, result.F);
  return result;
}

Mesh generateSteps(
  float width,
  float height,
  float stepSize,
  float maxHeight)
{
  std::default_random_engine generator;
  std::uniform_real_distribution<float> distribution(0.1, maxHeight);

  Mesh mesh;
  mesh.V = Eigen::MatrixXd((std::size_t)(4 * width/stepSize * height/stepSize), 3);
  mesh.F = Eigen::MatrixXi((std::size_t)(2 * width/stepSize * height/stepSize), 3);

  for(std::size_t i = 0; i < width/stepSize; i++)
  {
    for(std::size_t j = 0; j < height/stepSize; j++)
    {
      auto height = distribution(generator);
      mesh.V(4*(i*width + j), 0) = i * stepSize;
      mesh.V(4*(i*width + j), 1) = j * stepSize;
      mesh.V(4*(i*width + j), 2) = height;

      mesh.V(4*(i*width + j)+1, 0) = (i + 1) * stepSize;
      mesh.V(4*(i*width + j)+1, 1) = j * stepSize;
      mesh.V(4*(i*width + j)+1, 2) = height;

      mesh.V(4*(i*width + j)+2, 0) = i * stepSize;
      mesh.V(4*(i*width + j)+2, 1) = (j + 1) * stepSize;
      mesh.V(4*(i*width + j)+2, 2) = height;

      mesh.V(4*(i*width + j)+3, 0) = (i + 1) * stepSize;
      mesh.V(4*(i*width + j)+3, 1) = (j + 1) * stepSize;
      mesh.V(4*(i*width + j)+3, 2) = height;

      mesh.F(2 * (i*width + j), 0) = 4*(i*width + j);
      mesh.F(2 * (i*width + j), 1) = 4*(i*width + j)+1;
      mesh.F(2 * (i*width + j), 2) = 4*(i*width + j)+2;
      mesh.F(2 * (i*width + j) + 1, 0) = 4*(i*width + j)+1;
      mesh.F(2 * (i*width + j) + 1, 1) = 4*(i*width + j)+2;
      mesh.F(2 * (i*width + j) + 1, 2) = 4*(i*width + j)+3;
    }
  }
  return mesh;
}

int main(int argc, char *argv[])
{
  // Inline mesh of a cube
  /*const Eigen::MatrixXd V= (Eigen::MatrixXd(8,3)<<
    0.0,0.0,0.0,
    0.0,0.0,1.0,
    0.0,1.0,0.0,
    0.0,1.0,1.0,
    1.0,0.0,0.0,
    1.0,0.0,1.0,
    1.0,1.0,0.0,
    1.0,1.0,1.0).finished();
  const Eigen::MatrixXi F = (Eigen::MatrixXi(12,3)<<
    1,7,5,
    1,3,7,
    1,4,3,
    1,2,4,
    3,8,7,
    3,4,8,
    5,7,8,
    5,8,6,
    1,5,6,
    1,6,2,
    2,6,8,
    2,8,4).finished().array()-1;*/

  auto result = generateSteps(
    20, 20, 1, 0.4
  );
  // Plot the mesh
  igl::opengl::glfw::Viewer viewer;
  viewer.data().set_mesh(result.V, result.F);
  viewer.data().set_face_based(true);
  viewer.launch();
}
