﻿// <copyright file="LanguageClient.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright>

using System;
using System.IO;
using System.Net.Http;
using System.Net.Http.Headers;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization.Metadata;
using System.Threading;
using System.Threading.Tasks;

namespace Soup.Build.Api.Client;

/// <summary>
/// The closure client.
/// </summary>
public class ClosureClient
{
	private readonly HttpClient httpClient;
	private readonly string? bearerToken;

	public ClosureClient(HttpClient httpClient, string? bearerToken)
	{
		this.httpClient = httpClient;
		this.bearerToken = bearerToken;
	}

	public Uri BaseUrl { get; init; } = new Uri("https://api.soupbuild.com");

	/// <summary>
	/// Generate a package closure.
	/// </summary>
	/// <returns>The action result.</returns>
	/// <exception cref="ApiException">A server side error occurred.</exception>
	public virtual Task<GenerateClosureResultModel> GenerateClosureAsync(GenerateClosureRequestModel request)
	{
		return GenerateClosureAsync(request, CancellationToken.None);
	}

	/// <param name="request"></param>
	/// <param name="cancellationToken">A cancellation token that can be used by other objects or threads to receive notice of cancellation.</param>
	/// <summary>
	/// Generate a package closure.
	/// </summary>
	/// <returns>The action result.</returns>
	/// <exception cref="ApiException">A server side error occurred.</exception>
	public virtual async Task<GenerateClosureResultModel> GenerateClosureAsync(
		GenerateClosureRequestModel request, CancellationToken cancellationToken)
	{
		var urlBuilder_ = new StringBuilder();
		_ = urlBuilder_.Append(this.BaseUrl.OriginalString.TrimEnd('/')).Append("/v1/closure/generate");

		var client = this.httpClient;

		using var requestMessage = await CreateHttpRequestMessageAsync().ConfigureAwait(false);
		using var jsonContent = new MemoryStream();
		await JsonSerializer.SerializeAsync(
			jsonContent, request, SourceGenerationContext.Default.GenerateClosureRequestModel, cancellationToken);
		_ = jsonContent.Seek(0, SeekOrigin.Begin);

		using var content = new StreamContent(jsonContent);
		content.Headers.ContentType = MediaTypeHeaderValue.Parse("application/json");

		requestMessage.Content = content;
		requestMessage.Method = new HttpMethod("GET");
		requestMessage.Headers.Accept.Add(MediaTypeWithQualityHeaderValue.Parse("application/json"));

		var url = urlBuilder_.ToString();
		requestMessage.RequestUri = new Uri(url, UriKind.RelativeOrAbsolute);

		using var response = await client.SendAsync(
			requestMessage, HttpCompletionOption.ResponseHeadersRead, cancellationToken).ConfigureAwait(false);

		var status = (int)response.StatusCode;
		if (status == 200)
		{
			var objectResponse = await ReadObjectResponseAsync(
				response,
				SourceGenerationContext.Default.GenerateClosureResultModel,
				cancellationToken).ConfigureAwait(false);
			return objectResponse;
		}
		else
		{
			throw new ApiException("The HTTP status code of the response was not expected.", response.StatusCode, null, null);
		}
	}

	protected virtual async Task<T> ReadObjectResponseAsync<T>(
		HttpResponseMessage response,
		JsonTypeInfo<T> jsonTypeInfo,
		CancellationToken cancellationToken)
	{
		try
		{
			using var responseStream = await response.Content.ReadAsStreamAsync(cancellationToken).ConfigureAwait(false);
			var typedBody = await JsonSerializer.DeserializeAsync(
				responseStream, jsonTypeInfo, cancellationToken).ConfigureAwait(false);
			if (typedBody is null)
			{
				var message = "Response body was empty.";
				throw new ApiException(message, response.StatusCode, null, null);
			}

			return typedBody;
		}
		catch (JsonException exception)
		{
			var message = "Could not deserialize the response body stream as " + typeof(T).FullName + ".";
			throw new ApiException(message, response.StatusCode, null, exception);
		}
	}

	/// <summary>
	/// Called by implementing swagger client classes.
	/// </summary>
	protected Task<HttpRequestMessage> CreateHttpRequestMessageAsync()
	{
		var request = new HttpRequestMessage();
		if (!string.IsNullOrEmpty(this.bearerToken))
		{
			request.Headers.Authorization = new AuthenticationHeaderValue("Bearer", this.bearerToken);
		}

		return Task.FromResult(request);
	}
}