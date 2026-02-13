Version: 6
Closure: {
	C: {
		'mwasplund|libseccomp': { Version: 2.5.9, Digest: 'sha256:13d4dd00af9758bacffac95d987071ecbdfa994de2d86e331dd7f81318bed243', Build: '3', Tool: '0' }
		'mwasplund|wren': { Version: 1.2.0, Digest: 'sha256:6b867a0b48ad53165921bc734032dc245b045898683e744c4df74c2ee06faecc', Build: '3', Tool: '0' }
	}
	'C#': {
		Opal: { Version: '../../generate-sharp/opal/', Build: '0', Tool: '0' }
		'Soup.Bootstrap': { Version: './', Build: '0', Tool: '0' }
		'Soup.Native.Interop': { Version: '../../generate-sharp/soup-native-interop/', Build: '1', Tool: '0' }
		'Soup.Utilities': { Version: '../../generate-sharp/utilities/', Build: '1', Tool: '0' }
	}
	'C++': {
		'Monitor.Host': { Version: '../../monitor/host/', Build: '2', Tool: '1' }
		'Monitor.Shared': { Version: '../../monitor/shared/', Build: '2', Tool: '1' }
		'Soup.Core': { Version: '../../client/core/', Build: '2', Tool: '1' }
		'Soup.Native': { Version: '../../client/native/', Build: '2', Tool: '1' }
		'mwasplund|CryptoPP': { Version: 1.2.7, Digest: 'sha256:354692b27aae43193129cf3607e3ced863f980083de84dddc0a7d6203f471e45', Build: '2', Tool: '1' }
		'mwasplund|Detours': { Version: 4.0.14, Digest: 'sha256:21c5dcdc2b06bf8fdfb998035130d27debf060046e4fa0ed464a6d8aed3ba234', Build: '2', Tool: '1' }
		'mwasplund|Opal': { Version: 0.12.4, Digest: 'sha256:434f3d48248033b798961c6047a48106d5a78407a9730958516a0090f7167e68', Build: '2', Tool: '1' }
		'mwasplund|Soup.Test.Assert': { Version: 0.5.0, Digest: 'sha256:54b1d3506ee764936c0ba81388853854bfd328b6e7292e2486212f5c8596b570', Build: '2', Tool: '1' }
		'mwasplund|json11': { Version: 1.1.5, Digest: 'sha256:f646ce4a624262be6608142c1df17e72480ec413c36c100eb6b44257d09173da', Build: '2', Tool: '1' }
		'mwasplund|reflex': { Version: 5.5.2, Digest: 'sha256:96bf955a7f231259e49faa821cd9f5fe4610b146038f5c6a3c8e91c8192d747c', Build: '2', Tool: '1' }
	}
}
Builds: {
	0: {
		Wren: {
			'Soup|CSharp': {
				Version: 0.17.0
				Digest: 'sha256:5f8a05aedfc9d1c6857213367c70febd008b3d5e8c98f510e898177676ebd65a'
			}
		}
	}
	1: {
		Wren: {
			'Soup|CSharp': {
				Version: 0.17.0
				Digest: 'sha256:5f8a05aedfc9d1c6857213367c70febd008b3d5e8c98f510e898177676ebd65a'
			}
			'Soup|CSharp.Nuget': {
				Version: 0.7.0
				Digest: 'sha256:36014a673aeb99acc53f46b25011f5859e36631d6a195ca887722093cd047c26'
			}
		}
	}
	2: {
		Wren: {
			'Soup|Cpp': {
				Version: 0.16.1
				Digest: 'sha256:ba382dc10cad04e619a7d3b069c5936e5dfc2ab3f7cfab439b6e3ee9149248b1'
				Artifacts: {
					Linux: 'sha256:71d6886c5c78f9ef813aea3e02529d69bcf442a9d73a0026454d6b12f11d40ef'
				}
			}
		}
	}
	3: {
		Wren: {
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
	1: {
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
			'mwasplund|parse.modules': {
				Version: 1.2.1
				Digest: 'sha256:956daeffcd9ad54e9b33ca3430c70868924c348be8d0d50b64c904d7acc5b97c'
				Artifacts: {
					Linux: 'sha256:d86953a2cc37ee7ae140cbcde11c2154725324b4cfe5e5763024b307e5630af3'
				}
			}
		}
	}
}