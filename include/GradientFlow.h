#include "LinearSystem.h"
//#include "gnuplot-iostream.h"

using namespace dealii;

class GradientFlow
{
public:

  GradientFlow();

  //unsigned int get_vectors_dimension() const { return dim_vec; };
  const Vector<double>& get_y_vec() const { return y_vec; };
  const Vector<double>& get_u_vec() const { return u_vec; };
  //const std::tuple<Vector<double>, Vector<double>>& get_vectors() const { return std::make_tuple(y_vec, u_vec); };
  void set_initial_vectors(const Vector<double>& y0, const Vector<double>& u0);
  void set_step_size(const double step_size);
  void run(const unsigned int n_iter=1);
  void output_results_vectors() const;
  void output_iteration_results() const;

private:

  void initialize_dimensions();
  void vectors_iteration_step();
  void Adam_learning_rate();
  void momentum();
  double evaluate_J();
  void output_convergence_plots(const std::vector<double>& J_vec, const std::vector<double>& phi1_norm, const std::vector<double>& phi2_norm) const;

  LinearSystem          linear_system;

  unsigned int          dim_vec;

  double                h=1;

  Vector<double>        y_vec;
  Vector<double>        u_vec;

  Vector<double>        phi1;
  Vector<double>        phi2;

};

GradientFlow::GradientFlow()
:
  linear_system()
  ,dim_vec(linear_system.get_vector_size())
{
  initialize_dimensions();
}


void GradientFlow::initialize_dimensions()
{
  y_vec.reinit(dim_vec);
  u_vec.reinit(dim_vec);
  phi1.reinit(dim_vec);
  phi2.reinit(dim_vec);
}

void GradientFlow::set_initial_vectors(const Vector<double>& y0, const Vector<double>& u0)
{
  assert(y0.size() == dim_vec);
  assert(u0.size() == dim_vec);
  y_vec = y0;
  u_vec = u0;

  linear_system.update_vectors(y_vec,u_vec);
}


void GradientFlow::vectors_iteration_step()
{
  linear_system.solve_system();

  phi1 = linear_system.get_phi1();
  phi2 = linear_system.get_phi2();

  y_vec.add(h, phi1);
  u_vec.add(h, phi2);

  linear_system.update_vectors(y_vec,u_vec);
}


void GradientFlow::run(const unsigned int n_iter)
{
  unsigned int k = 0;
  Vector<double>   g(dim_vec);


  while (k < n_iter)
  {
    vectors_iteration_step();
    g = linear_system.get_g();
    //std::cout << std::setprecision(11) << linear_system.evaluate_J() << " g: " << g.l2_norm() << std::endl;

    k++;
  }
  /*
  //while (k_iter == 0 or (phi1.l2_norm() > 1e-3 and phi2.l2_norm() > 1e-3))
  while (k_iter < n_iter)
  {
    linear_system.output_result_vectors();
    vectors_iteration_step();
    J_vec.push_back(linear_system.evaluate_J());
    phi1_norm.push_back(phi1.l2_norm());
    phi2_norm.push_back(phi2.l2_norm());
    g = linear_system.get_g();
    std::cout << std::setprecision(11) << linear_system.evaluate_J() << " g: " << g.l2_norm() << std::endl;

    if (k_iter%100 == 0)
      linear_system.output_result_vectors();
    //if (J_vec[k_iter] < 0.9896885818)
    //  h = 0.01;

    //std::cout << "norm phi1: " << phi1.l2_norm() << std::endl;
    //std::cout << "norm phi2: " << phi2.l2_norm() << std::endl;
    k_iter++;
  }

  std::cout << "norm phi1: " << phi1.l2_norm() << std::endl;
  std::cout << "norm phi2: " << phi2.l2_norm() << std::endl;

  linear_system.update_vectors(y_vec,u_vec);
  linear_system.output_result_vectors();

  output_convergence_plots(J_vec, phi1_norm, phi2_norm);
  */
}

void GradientFlow::output_iteration_results() const
{
  std::cout << std::setprecision(11) << linear_system.evaluate_J() << " g: " << linear_system.evaluate_g() << std::endl;
}

void GradientFlow::set_step_size(const double step_size)
{
  h = step_size;
}

void GradientFlow::output_results_vectors() const
{
  linear_system.output_result_vectors();
}

void GradientFlow::output_convergence_plots(const std::vector<double>& J_vec, const std::vector<double>& phi1_norm, const std::vector<double>& phi2_norm) const
{
  std::ofstream file;
  file.open("J_evaluation.dat");
  for (unsigned int iter=0; iter<J_vec.size(); iter++)
  {
      file << iter << " " << J_vec[iter] << std::endl;
  }
  file.close();

  file.open("phi1_norm.dat");
  for (unsigned int iter=0; iter<phi1_norm.size(); iter++)
  {
      file << iter << " " << phi1_norm[iter] << std::endl;
  }
  file.close();

  file.open("phi2_norm.dat");
  for (unsigned int iter=0; iter<phi2_norm.size(); iter++)
  {
      file << iter << " " << phi2_norm[iter] << std::endl;
  }
  file.close();
  /*
  Gnuplot gp;
  gp << "set terminal png " << std::endl;
  gp << "set grid" << std::endl;
  gp << "set output 'J_eval.png' " << std::endl;
  gp << "set ylabel 'n_iter'" << std::endl;
  gp << "set xlabel 'J'" << std::endl;
  gp << "set multiplot layout 1,2" << std::endl;
  gp << "plot 'J_evaluation.dat' with lp lw 0.5 title 'Evaluation Loss Function J'" << std::endl;
  gp << "unset multiplot" << std::endl;
*/
}
