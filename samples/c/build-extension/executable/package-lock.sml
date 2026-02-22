Version: 6
Closure: {
	C: {
		'Samples.C.SimpleBuildExtension.Executable': { Version: './', Build: '0', Tool: '0' }
	}
}
Builds: {
	0: {
		Wren: {
			'Samples.C.BuildExtension.Extension': { Version: '../extension/' }
			'Soup|C': {
				Version: 0.5.0
				Digest: 'sha256:1e9683087b84ed998a5ea4a96ee70561e4af5575f46c68d4cdf8a154d405d52b'
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