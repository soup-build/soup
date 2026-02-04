Version: 5
Closures: {
	Root: {
		'C++': {
			copy: { Version: './', Build: 'Build0', Tool: 'Tool0' }
		}
	}
	Build0: {
		Wren: {
			'soup|Cpp': { Version: 0.16.1, Digest: 'sha256:ba382dc10cad04e619a7d3b069c5936e5dfc2ab3f7cfab439b6e3ee9149248b1' }
		}
	}
	Tool0: {
		'C++': {
			'mwasplund|mkdir': { Version: 1.2.0, Digest: 'sha256:14bdf971c3a955c79982a7e18d248516c4a70bd6b139abb286f44d80d5e91ac2' }
			'mwasplund|copy': { Version: 1.2.0, Digest: 'sha256:4fd4849523e50e06930c6f97088827feeae6b52ad6020bfbda578f37084234bf' }
			'mwasplund|parse.modules': { Version: 1.2.1, Digest: 'sha256:956daeffcd9ad54e9b33ca3430c70868924c348be8d0d50b64c904d7acc5b97c' }
		}
	}
}