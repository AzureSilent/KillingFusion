//
// Created by Saurabh Khanduja on 26.10.18.
//

#ifndef INC_3DSCANNINGANDMOTIONCAPTURE_SDF_H
#define INC_3DSCANNINGANDMOTIONCAPTURE_SDF_H

#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "DisplacementField.h"
#include "SimpleMesh.h"

// ToDo: SDF should take real world coordinates and return the distance. Not the world coordinates in voxel coordinates.
class SDF
{
  Eigen::Vector3i m_gridSize;
  Eigen::Vector3f m_min3dLoc;
  Eigen::Vector3f m_max3dLoc;
  int m_totalNumberOfVoxels;
  // ToDo - Change to vector of vector of vector.
  // Makes notation much simpler as well as reduces the computation of indices.
  std::vector<float> m_voxelGridTSDF;
  std::vector<long> m_voxelGridWeight;
  float m_voxelSize; // ToDo: Remove this m_voxelSize. Use directly from config.h
  Eigen::Vector3i m_gridSpacingPerAxis;
  Eigen::Vector3f m_bound;
  float m_unknownClipDistance;

  void computeVoxelGridSize();
  void allocateMemoryForSDF();
  bool ProcessVolumeCell(int x, int y, int z, double iso, SimpleMesh *mesh) const;

public:
  // ToDo: Remove use of truncationDistanceInVoxelSize and add a method getTruncatedDistance.
  SDF(float _voxelSize,
      Eigen::Vector3f _min3dLoc,
      Eigen::Vector3f _max3dLoc,
      float unknownClipDistance);

  // Creates a new SDF of same size as copy. Voxel weight and distance is not copied.
  SDF(const SDF &copy)
  noexcept;

  SDF(SDF &&f)
  noexcept;

  ~SDF();

  static std::vector<SDF> getDataEnergyTestSample(float _voxelSize,
                                                  float unknownClipDistance);

  /**
     * Main function to merge depth frames into the SDF volume.
     * The method can be called multiple times but need to be initialized with volume bounds beforehand.
     * @param depthFrame
     * @param depthFrameC2WPose
     * @param depthIntrinsicMatrix
     * @param minDepth
     * @param maxDepth
     */
  void integrateDepthFrame(cv::Mat depthFrame,
                           Eigen::Matrix4f depthFrameC2WPose,
                           Eigen::Matrix3f depthIntrinsicMatrix,
                           float minDepth,
                           float maxDepth);

  /**
   * Provides an easy way to check if an index(0-index) is within bound of the SDF grid.
   */
  bool indexInGridBounds(int x, int y, int z) const;
  bool indexInGridBounds(const Eigen::Vector3i &gridSpatialIndex) const;

  /**
   * Get SDF distance value at spatial index(0-indexed).
   */
  float getDistanceAtIndex(const Eigen::Vector3i &gridSpatialIndex) const;

  /**
   * Get SDF distance value at spatial index(0-indexed).
   */
  float getDistanceAtIndex(int x, int y, int z) const;

  /**
   * Get SDF weight value at spatial index.
   */
  long getWeightAtIndex(const Eigen::Vector3i &gridSpatialIndex) const;

  /**
   * Get SDF weight value at spatial index.
   */
  long getWeightAtIndex(int x, int y, int z) const;

  /**
   * Get SDF distance value at grid location. Grid Location unit is voxel size.
   */
  float getDistance(const Eigen::Vector3f &gridLocation) const;

  static void testGetDistance();

  /**
   * Get distance value at grid location of displaced SDF. Grid Location unit is voxel size.
   */
  float getDistance(const Eigen::Vector3i &spatialIndex,
                    const DisplacementField *displacementField) const;

  float getDistancef(const Eigen::Vector3f &gridLocation,
                    const DisplacementField *displacementField) const;

  /**
   * Get weight value at grid location of SDF. Grid Location unit is voxel size.
   */
  float getWeight(const Eigen::Vector3f &gridLocation) const;

  static void testGetWeight();

  float getWeight(const Eigen::Vector3i &spatialIndex,
                  const DisplacementField *displacementField) const;

  /**
   * This function computes distance gradient at any grid location. Grid Location unit is voxel size.
   */
  Eigen::Vector3f computeDistanceGradient(const Eigen::Vector3f &gridLocation) const;

  static void testComputeDistanceGradient();
  Eigen::Vector3f computeDistanceGradient(const Eigen::Vector3i &spatialIndex,
                                          const DisplacementField *displacementField) const;

  /**
   * This function computes distance hessian at any grid location. Grid Location unit is voxel size.
   */
  Eigen::Matrix3f computeDistanceHessian(const Eigen::Vector3f &gridLocation) const;

  Eigen::Matrix3f computeDistanceHessian(const Eigen::Vector3i &spatialIndex,
                                         const DisplacementField *displacementField) const;

  /**
   * Fuses otherSdf which should be of same size as this.
   */
  void fuse(const SDF *otherSdf);

  /**
   * Fuses otherSdf using its DisplacementField
   */
  void fuse(const SDF *otherSdf, const DisplacementField *otherDisplacementField);

  /**
   * This methods applies the displacement to the SDF.
   */
  void update(const DisplacementField *displacementField);

  /**
   * Dumps mesh of SDF using marching cubes algorithm.
   */
  void save_mesh(std::string mesh_name_prefix, int fileCounter) const;

  /**
   * Returns mesh of SDF using marching cubes algorithm.
   */
  SimpleMesh *getMesh() const;
  SimpleMesh *getMesh(const DisplacementField &displacementField) const;

  /**
   * Dumps mesh of SDF with deformation field applied using marching cubes algorithm.
   */
  void save_mesh(std::string mesh_name_prefix,
                 int fileCounter,
                 const DisplacementField &displacementField) const;

  /**
   * Writes the SDF to a file.
   * @param outputFilePath
   * @param truncationDistanceInVoxelSizeUnit - Not used
   * @param minimumWeightThreshold - Not used
   */
  void dumpToBinFile(std::string outputFilePath,
                     float truncationDistanceInVoxelSizeUnit,
                     float minimumWeightThreshold) const;

  /**
   * Getters
   */
  Eigen::Vector3f getMin3dLoc() const
  {
    return m_min3dLoc;
  };

  Eigen::Vector3f getMax3dLoc() const
  {
    return m_max3dLoc;
  };

  Eigen::Vector3i getGridSize() const
  {
    return m_gridSize;
  };
};

#endif //INC_3DSCANNINGANDMOTIONCAPTURE_SDF_H
