/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file Pose2SLAMExample.cpp
 * @brief A 2D Pose SLAM example that reads input from g2o and uses robust kernels in optimization
 * @date May 15, 2014
 * @author Luca Carlone
 */

#include <gtsam/geometry/Pose2.h>
#include <gtsam/inference/Key.h>
#include <gtsam/slam/PriorFactor.h>
#include <gtsam/slam/dataset.h>
#include <gtsam/slam/BetweenFactor.h>
#include <gtsam/nonlinear/NonlinearFactorGraph.h>
#include <gtsam/nonlinear/GaussNewtonOptimizer.h>
#include <gtsam/nonlinear/Marginals.h>
#include <gtsam/nonlinear/Values.h>
#include <fstream>
#include <sstream>

using namespace std;
using namespace gtsam;

#define LINESIZE 81920

int main(const int argc, const char *argv[]){

  if (argc < 2)
    std::cout << "Please specify input file (in g2o format) and output file" << std::endl;
  const string g2oFile = argv[1];

  NonlinearFactorGraph graph;
  Values initial;
  readG2o(g2oFile, graph, initial);

  // otherwise GTSAM cannot solve the problem
  NonlinearFactorGraph graphWithPrior = graph;
  noiseModel::Diagonal::shared_ptr priorModel = noiseModel::Diagonal::Variances((Vector(3) << 0.01, 0.01, 0.001));
  graphWithPrior.add(PriorFactor<Pose2>(0, Pose2(), priorModel));

  // Create the optimizer ...
  std::cout << "Optimizing the factor graph" << std::endl;
  GaussNewtonOptimizer optimizer(graphWithPrior, initial); // , parameters);
  // ... and optimize
  Values result = optimizer.optimize();
  std::cout << "Optimization complete" << std::endl;

  const string outputFile = argv[2];
  std::cout << "Writing results to file: " << outputFile << std::endl;

  noiseModel::Diagonal::shared_ptr model = noiseModel::Diagonal::Sigmas((Vector(3) << 0.0, 0.0, 0.0));
  writeG2o(graph, result, model, outputFile);
  std::cout << "done! " << std::endl;

  return 0;
}
