Version: 6
Closure: {
	C: {
		'mwasplund|libseccomp': { Version: 2.5.9, Digest: 'sha256:13d4dd00af9758bacffac95d987071ecbdfa994de2d86e331dd7f81318bed243', Build: '1', Tool: '0' }
		'mwasplund|wren': { Version: 1.2.0, Digest: 'sha256:6b867a0b48ad53165921bc734032dc245b045898683e744c4df74c2ee06faecc', Build: '1', Tool: '0' }
	}
	'C++': {
		'Monitor.Host': { Version: '../../monitor/host/', Build: '0', Tool: '0' }
		'Monitor.Shared': { Version: '../../monitor/shared/', Build: '0', Tool: '0' }
		'Soup.Core': { Version: '../core/', Build: '0', Tool: '0' }
		'Soup.Native': { Version: './', Build: '0', Tool: '0' }
		'mwasplund|CryptoPP': { Version: 1.2.7, Digest: 'sha256:354692b27aae43193129cf3607e3ced863f980083de84dddc0a7d6203f471e45', Build: '0', Tool: '0' }
		'mwasplund|Detours': { Version: 4.0.14, Digest: 'sha256:21c5dcdc2b06bf8fdfb998035130d27debf060046e4fa0ed464a6d8aed3ba234', Build: '0', Tool: '0' }
		'mwasplund|Opal': { Version: 0.12.4, Digest: 'sha256:434f3d48248033b798961c6047a48106d5a78407a9730958516a0090f7167e68', Build: '0', Tool: '0' }
		'mwasplund|Soup.Test.Assert': { Version: 0.5.0, Digest: 'sha256:54b1d3506ee764936c0ba81388853854bfd328b6e7292e2486212f5c8596b570', Build: '0', Tool: '0' }
		'mwasplund|json11': { Version: 1.1.5, Digest: 'sha256:f646ce4a624262be6608142c1df17e72480ec413c36c100eb6b44257d09173da', Build: '0', Tool: '0' }
		'mwasplund|reflex': { Version: 5.5.2, Digest: 'sha256:96bf955a7f231259e49faa821cd9f5fe4610b146038f5c6a3c8e91c8192d747c', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'Soup|Cpp': {
				Version: 0.16.1
				Digest: 'sha256:ba382dc10cad04e619a7d3b069c5936e5dfc2ab3f7cfab439b6e3ee9149248b1'
				Artifacts: {
					Linux: 'sha256:71d6886c5c78f9ef813aea3e02529d69bcf442a9d73a0026454d6b12f11d40ef'
					Windows: 'sha256:2a2ef40709982ecc5b2451b47ef2c54f7a5b27b04e54bc00db5cf8245e4fdee6'
				}
			}
		}
	}
	'1': {
		Wren: {
			'Soup|C': {
				Version: 0.5.0
				Digest: 'sha256:1e9683087b84ed998a5ea4a96ee70561e4af5575f46c68d4cdf8a154d405d52b'
				Artifacts: {
					Windows: 'sha256:921e9035c31d2e392a2920289e74b4ed4302d8020c172caddee874ec861260f8'
				}
			}
		}
	}
}
Tools: {
	'0': {
		'C++': {
			'mwasplund|copy': {
				Version: 1.2.0
				Digest: 'sha256:4fd4849523e50e06930c6f97088827feeae6b52ad6020bfbda578f37084234bf'
				Artifacts: {
					Linux: 'sha256:60a9a23a9eea9c4c767b4c4dc4b5738017ff2592426de7215c3b8b14e439d832'
					Windows: 'sha256:b7b38403b6cf786d484b6ca68b1b8158197ef8546dd73f5c6a681d0ed3b75f1c'
				}
			}
			'mwasplund|mkdir': {
				Version: 1.2.0
				Digest: 'sha256:14bdf971c3a955c79982a7e18d248516c4a70bd6b139abb286f44d80d5e91ac2'
				Artifacts: {
					Linux: 'sha256:c3962d814e1c716454b803753f8304af8bb85b6260f6802992781fb8ae4c5365'
					Windows: 'sha256:ca96d73393fb552312190c3392490e3c61e2e4e0e5e50e411d5924fd27e34eba'
				}
			}
			'mwasplund|parse.modules': {
				Version: 1.2.1
				Digest: 'sha256:956daeffcd9ad54e9b33ca3430c70868924c348be8d0d50b64c904d7acc5b97c'
				Artifacts: {
					Linux: 'sha256:d86953a2cc37ee7ae140cbcde11c2154725324b4cfe5e5763024b307e5630af3'
					Windows: 'sha256:af04a022169ee2d0e7fb932c1ee253b4f2eff71afc1b4222c4f5203e53ad02d4'
				}
			}
		}
	}
}