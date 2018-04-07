#include "gtest/gtest.h"

#include "boxqp.h"

static const double eq_tol = 1e-6;

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::Matrix2d;
using Eigen::Matrix3d;
using Eigen::Vector2d;
using Eigen::Vector3d;
using Eigen::Vector2i;
using Eigen::VectorXi;

TEST(BoxQpTest, ClampTest)
{
  Vector3d x(20.0, -50.0, 1.0);
  Vector3d lower(-10.0, -10.0, -10.0);
  Vector3d upper(5.0, 5.0, 5.0);

  VectorXd x_clamped = clamp_to_limits(x, lower, upper);
  EXPECT_TRUE(x_clamped.isApprox(Vector3d(5.0, -10.0, 1.0), eq_tol));
}

TEST(BoxQpTest, SubvecWIndTest)
{
	VectorXd vec(5), expected(3);
  VectorXi indices(5);
	vec << 20.0, -50.0, 1.0, 9.0, 11.0;
	indices << 1, 1, 0, 1, 0;
	expected << 20.0, -50.0, 9.0;

	VectorXd subvec = subvec_w_ind(vec, indices);
	EXPECT_TRUE(subvec.isApprox(expected, eq_tol));
}

TEST(BoxQpTest, QuadCostTest)
{
	Vector2d x(0.35, 0.7);
	Matrix2d H;
	H << 0.25, 0.,
			 0, 0.6;
	Vector2d g(-15., 1.);

	double val = quadCost(H, g, x);
	EXPECT_TRUE(std::abs(val-(-4.3876875)) < eq_tol);
}

TEST(BoxQpTest, LineSearchTest1) // easy case
{
	Vector2d x0(2., 2.);
	Vector2d dir(-1., -1.);
	Matrix2d H;
	H << 2, 0,
			 0, 2;
	Vector2d g(0., 0.);
	Vector2d lower(-10., -10.);
	Vector2d upper(10., 10.);

	lineSearchResult res = quadclamp_line_search(x0, dir, H, g, lower, upper);
	// std::cout << "x:\n" << res.x_opt << std::endl;
	// std::cout << "v: " << res.v_opt << std::endl;
	// std::cout << "nsteps: " << res.n_steps << std::endl;
	// std::cout << "failed: " << res.failed << std::endl;

	EXPECT_TRUE(res.x_opt.isApprox(Vector2d(1., 1.), eq_tol));
	EXPECT_TRUE(std::abs(res.v_opt - 2) < eq_tol);
	EXPECT_FALSE(res.failed);
}

TEST(BoxQpTest, LineSearchTest2) // positive local slope - search direction wrong
{
	Vector2d x0(2., 2.);
	Vector2d dir(1., 1.);
	Matrix2d H;
	H << 2, 0,
			 0, 2;
	Vector2d g(0., 0.);
	Vector2d lower(-10., -10.);
	Vector2d upper(10., 10.);

	lineSearchResult res = quadclamp_line_search(x0, dir, H, g, lower, upper);
	EXPECT_TRUE(res.failed);
}

TEST(BoxQpTest, LineSearchTest3) // hit limits
{
	Vector2d x0(2., 2.);
	Vector2d dir(-1., -1.);
	Matrix2d H;
	H << 2, 0,
			 0, 2;
	Vector2d g(0., 0.);
	Vector2d lower(1.5, 1.5);
	Vector2d upper(10., 10.);

	lineSearchResult res = quadclamp_line_search(x0, dir, H, g, lower, upper);
  EXPECT_TRUE(res.x_opt.isApprox(Vector2d(1.5, 1.5), eq_tol));
  EXPECT_TRUE(std::abs(res.v_opt - 4.5) < eq_tol);
	EXPECT_FALSE(res.failed);
}

// boxQPResult boxQP(const MatrixXd &H, const VectorXd &g, const VectorXd &x0,
//                   const VectorXd& lower, const VectorXd& upper)

// int result = 0;
// VectorXd x_opt;
// VectorXd v_free;
// MatrixXd R_free;

TEST(BoxQpTest, BoxQpTest1) // easy case
{
  Vector2d x0(2., 2.);
	Matrix2d H;
	H << 2, 0,
		   0, 2;
	Vector2d g(0., 0.);
	Vector2d lower(-10., -10.);
	Vector2d upper(10., 10.);

  boxQPResult res = boxQP(H, g, x0, lower, upper);
  // std::cout << "v_free\n" << res.v_free << std::endl;
  // std::cout << "R_free\n" << res.R_free << std::endl;

  // EXPECT_EQ(res.result, 0);
  EXPECT_TRUE(res.x_opt.isApprox(Vector2d(0., 0.), eq_tol));
}

TEST(BoxQpTest, BoxQpTest2) // hit limits
{
  Vector2d x0(2., 2.);
  Matrix2d H;
  H << 2, 0,
       0, 2;
  Vector2d g(0., 0.);
  Vector2d lower(1.5, 1.5);
  Vector2d upper(10., 10.);

  boxQPResult res = boxQP(H, g, x0, lower, upper);
  // std::cout << "result: " << res.result << std::endl;
  // std::cout << "x_free\n" << res.x_opt << std::endl;
  // std::cout << "v_free\n" << res.v_free << std::endl;
  // std::cout << "R_free\n" << res.R_free << std::endl;

  Matrix2d H_exp;
  H_exp << 1.41421, 0,
      	   0, 1.41421;

  EXPECT_EQ(res.result, 6);
  EXPECT_TRUE(res.x_opt.isApprox(Vector2d(1.5, 1.5), eq_tol));
  EXPECT_TRUE(res.v_free == Vector2i(0, 0));
  EXPECT_TRUE(res.R_free.isApprox(H_exp, 1e-3));
}

TEST(BoxQpTest, BoxQpTest3)
{
  Vector2d x0(0., 0.);
  Matrix2d H;
  H << 3.001, 0,
       0, 3.001;
  Vector2d g(0.201, 0.201);
  Vector2d lower(-0.6, -0.6);
  Vector2d upper(0.4, 0.4);

  boxQPResult res = boxQP(H, g, x0, lower, upper);
  // std::cout << "result: " << res.result << std::endl;
  // std::cout << "x_free\n" << res.x_opt << std::endl;
  // std::cout << "v_free\n" << res.v_free << std::endl;
  // std::cout << "R_free\n" << res.R_free << std::endl;

  Matrix2d H_exp;
  H_exp << 1.73234, 0,
      	   0, 1.73234;

  EXPECT_EQ(res.result, 5);
  EXPECT_TRUE(res.x_opt.isApprox(Vector2d(-0.0669777, -0.0669777), eq_tol));
  EXPECT_TRUE(res.v_free == Vector2i(1, 1));
  EXPECT_TRUE(res.R_free.isApprox(H_exp, 1e-3));
}

// Quadratic cost function around origin
// Clamped in one dimension. 
TEST(BoxQpTest, BoxQpTest4)
{
  Vector3d x0(0.5, 0.5, 1.0);
  Matrix3d H;
  Vector3d g;
  H.setIdentity();
  H(1, 1) = 5.0;
  g.setZero();

  Vector3d lower(0.2, -1.0, -1.0);
  Vector3d upper(1.0, 1.0, 1.0);

  boxQPResult res = boxQP(H, g, x0, lower, upper);

  std::cout << "result: " << res.result << std::endl;
  std::cout << "x_free: " << res.x_opt.transpose() << std::endl;
  std::cout << "v_free: " << res.v_free.transpose() << std::endl;
  std::cout << "R_free:\n" << res.R_free << std::endl;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
