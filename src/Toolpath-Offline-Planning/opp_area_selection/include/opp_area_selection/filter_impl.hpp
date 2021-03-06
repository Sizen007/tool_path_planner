/*
 * Copyright 2019 Southwest Research Institute
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OPP_AREA_SELECTION_FILTER_IMPL_H
#define OPP_AREA_SELECTION_FILTER_IMPL_H

#include "opp_area_selection/filter.h"

#include <pcl/filters/extract_indices.h>
#include <pcl/sample_consensus/ransac.h>
#include <pcl/sample_consensus/sac_model_plane.h>
#include <pcl/segmentation/sac_segmentation.h>

#include <ros/ros.h>

namespace opp_area_selection
{
namespace data_filtering
{
template <typename PointT>
bool planeFit(const CloudPtr<PointT> input_cloud,
              Cloud<PointT>& output_cloud,
              pcl::ModelCoefficients::Ptr plane_coefficients,
              const double threshold)
{
  pcl::ExtractIndices<PointT> extract;
  pcl::PointIndices::Ptr plane_inliers(new pcl::PointIndices);
  pcl::SACSegmentation<PointT> seg;

  // Set segmentation parameters
  seg.setOptimizeCoefficients(true);
  seg.setModelType(pcl::SACMODEL_PLANE);
  seg.setMethodType(pcl::SAC_RANSAC);
  seg.setDistanceThreshold(threshold);
  seg.setInputCloud(input_cloud);

  // Extract best fit cylinder inliers and calculate cylinder coefficients
  seg.segment(*plane_inliers, *plane_coefficients);

  if (plane_inliers->indices.size() == 0)
  {
    ROS_ERROR("Unable to fit a plane to the data");
    return false;
  }
  ROS_DEBUG("Plane fit: %lu input points, %lu output points",
	    input_cloud->points.size(),
	    plane_inliers->indices.size());

  // Create new point cloud from
  extract.setInputCloud(input_cloud);
  extract.setIndices(plane_inliers);
  extract.setNegative(false);
  extract.filter(output_cloud);

  return true;
}

}  // end namespace data_filtering

}  // end namespace opp_area_selection

#endif  // OPP_AREA_SELECTION_FILTER_IMPL_H
