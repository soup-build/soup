// <copyright file="load-build-graph-result.go" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

package main

type LoadBuildGraph struct {
	RootPackageGraphId int
	PackageGraphs []PackageGraph
	Packages []PackageInfo
	PackageTargetDirectories map[int]map[int]string
}

type LoadBuildGraphResult struct {
	IsSuccess bool
	Message *string
	Graph LoadBuildGraph
}