// <copyright file="ihttp-message-handler.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;
using System.Net.Http;

namespace Soup.Build.PackageManager.UnitTests;

public interface IHttpMessageHandler
{
	HttpResponseMessage Send(HttpMethod method, Uri requestUri, string headers, string? content);
	HttpResponseMessage SendAsync(HttpMethod method, Uri? requestUri, string headers, string? content);
}