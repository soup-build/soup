// <copyright file="package-with-artifact-reference.cpp" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

module;

#include <optional>
#include <format>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

export module Soup.Core:PackageWithArtifactReference;

import Opal;
import :Digest;
import :PackageIdentifier;
import :PackageReference;

using namespace Opal;

namespace Soup::Core
{
	/// <summary>
	/// A package reference object which will consist of a name version pair that
	/// refers to a published package and an optional artifact digest
	///  or a path to a local recipe
	/// </summary>
	export class PackageWithArtifactReference
	{
	private:
		PackageReference _packageReference;
		std::optional<Digest> _artifactDigest;

	public:
		/// <summary>
		/// Initializes a new instance of the <see cref="PackageWithArtifactReference"/> class.
		/// </summary>
		PackageWithArtifactReference() :
			_packageReference(),
			_artifactDigest(std::nullopt)
		{
		}

		/// <summary>
		/// Initializes a new instance of the <see cref="PackageWithArtifactReference"/> class.
		/// </summary>
		PackageWithArtifactReference(
			PackageReference packageReference,
			std::optional<Digest> artifactDigest) :
			_packageReference(std::move(packageReference)),
			_artifactDigest(std::move(artifactDigest))
		{
		}

		/// <summary>
		/// Gets or sets the PackageReference.
		/// </summary>
		const PackageReference& GetPackage() const
		{
			return _packageReference;
		}

		/// <summary>
		/// Gets or sets the artifact digest.
		/// </summary>
		bool HasArtifactDigest() const
		{
			return _artifactDigest.has_value();
		}

		/// <summary>
		/// Gets or sets the artifact digest.
		/// </summary>
		const Digest& GetArtifactDigest() const
		{
			if (!_artifactDigest.has_value())
				throw std::runtime_error("PackageWithArtifactReference does not have an artifact digest.");
			return _artifactDigest.value();
		}
		const std::optional<Digest>& GetArtifactDigestValue() const
		{
			return _artifactDigest;
		}

		/// <summary>
		/// Equality operator
		/// </summary>
		bool operator ==(const PackageWithArtifactReference& rhs) const
		{
			return _packageReference == rhs._packageReference &&
				_artifactDigest == rhs._artifactDigest;
		}

		/// <summary>
		/// Inequality operator
		/// </summary>
		bool operator !=(const PackageWithArtifactReference& rhs) const
		{
			return _packageReference != rhs._packageReference ||
				_artifactDigest != rhs._artifactDigest;
		}
	};
}
