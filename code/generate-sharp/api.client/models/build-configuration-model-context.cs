// <copyright file="build-configuration-model-context.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System.Text.Json.Serialization;

namespace Soup.Build.Api.Client;

[JsonSourceGenerationOptions(WriteIndented = true)]
[JsonSerializable(typeof(BuildConfigurationModel))]
public partial class BuildConfigurationModelContext : JsonSerializerContext { }
