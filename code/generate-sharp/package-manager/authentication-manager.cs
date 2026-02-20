// <copyright file="authentication-manager.cs" company="Soup">
// Copyright (c) Soup. All rights reserved.
// </copyright

using Duende.IdentityModel.OidcClient;
using Microsoft.Extensions.Logging;
using System.Globalization;
using System.Threading.Tasks;

namespace Soup.Build.PackageManager;

internal sealed class AuthenticationManager : IAuthenticationManager
{
	private const string _authority = "https://auth.soupbuild.com/";
	// private const string _authority = "https://auth.dev.soupbuild.com/";
	// private const string _authority = "https://localhost:5001/";

	/// <summary>
	/// Ensure the user is logged in
	/// </summary>
	/// <returns>The access token</returns>
	public async Task<string> EnsureSignInAsync(bool forceRefresh)
	{
		var token = await Login(forceRefresh);
		return token;
	}

	private static async Task<string> Login(bool forceRefresh)
	{
		// Create a redirect URI using an available port on the loopback address
		var browser = new SystemBrowser();
		string redirectUri = string.Format(CultureInfo.InvariantCulture, $"http://127.0.0.1:{browser.Port}");

		var options = new OidcClientOptions()
		{
			Authority = _authority,
			ClientId = "Soup.Native",
			RedirectUri = redirectUri,
			Scope = "openid profile soup_api",
			FilterClaims = false,
			Browser = browser,
			DisablePushedAuthorization = true,
			LoggerFactory = LoggerFactory.Create(builder =>
			{
				// TODO: Add verbose logging
				// _ = builder.AddConsole();
			}),
			// HttpClientFactory = (options) =>
			// {
			// 	var handler = new HttpClientHandler()
			// 	{
			// 		// Ignore SSL
			// 		ServerCertificateCustomValidationCallback = (sender, cert, chain, sslPolicyErrors) => true
			// 	};
			// 	return new HttpClient(handler);
			// }
		};

		var oidcClient = new OidcClient(options);

		if (forceRefresh)
		{
			_ = await oidcClient.LogoutAsync(new LogoutRequest());
		}

		var result = await oidcClient.LoginAsync(new LoginRequest());

		return result.AccessToken;
	}
}