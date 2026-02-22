Version: 6
Closure: {
	Wren: {
		'Samples.C.BuildExtension.Extension': { Version: './', Build: '0', Tool: '0' }
		'Soup|Build.Utils': { Version: 0.9.1, Digest: 'sha256:65d8d0415f37a762ab0e9d0e5597610bc9d84c5e595cf2bf7e81cf3701cb5ffc', Build: '0', Tool: '0' }
	}
}
Builds: {
	0: {
		Wren: {
			'Soup|Wren': {
				Version: 0.5.4
				Digest: 'sha256:bbd0bdf404cfdb4ec3c398e29072b10a32951b9be7c1e6b724d13ac17d703b03'
				Artifacts: {
					Linux: 'sha256:5186e1532f6bad21295d3f4b11fdd49d19ebd91090fe475ea294b8f92b8cc964'
				}
			}
		}
	}
}
Tools: {
	0: {
		'C++': {
			'mwasplund|copy': {
				Version: 1.2.0
				Digest: 'sha256:4fd4849523e50e06930c6f97088827feeae6b52ad6020bfbda578f37084234bf'
				Artifacts: {
					Linux: 'sha256:60a9a23a9eea9c4c767b4c4dc4b5738017ff2592426de7215c3b8b14e439d832'
				}
			}
			'mwasplund|mkdir': {
				Version: 1.2.0
				Digest: 'sha256:14bdf971c3a955c79982a7e18d248516c4a70bd6b139abb286f44d80d5e91ac2'
				Artifacts: {
					Linux: 'sha256:c3962d814e1c716454b803753f8304af8bb85b6260f6802992781fb8ae4c5365'
				}
			}
		}
	}
}