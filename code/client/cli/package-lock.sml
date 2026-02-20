Version: 6
Closure: {
	C: {
		'mwasplund|libseccomp': { Version: 2.5.9, Digest: 'sha256:13d4dd00af9758bacffac95d987071ecbdfa994de2d86e331dd7f81318bed243', Build: '2', Tool: '0' }
		'mwasplund|wren': { Version: 1.2.0, Digest: 'sha256:6b867a0b48ad53165921bc734032dc245b045898683e744c4df74c2ee06faecc', Build: '2', Tool: '0' }
	}
	'C++': {
		'Monitor.Host': { Version: '../../monitor/host/', Build: '0', Tool: '0' }
		'Monitor.Shared': { Version: '../../monitor/shared/', Build: '0', Tool: '0' }
		Soup: { Version: './', Build: '0', Tool: '0' }
		'Soup.Core': { Version: '../core/', Build: '1', Tool: '0' }
		'Soup.Generate': { Version: '../../generate/', Build: '0', Tool: '0' }
		'mwasplund|CryptoPP': { Version: 1.2.7, Digest: 'sha256:354692b27aae43193129cf3607e3ced863f980083de84dddc0a7d6203f471e45', Build: '0', Tool: '0' }
		'mwasplund|Detours': { Version: 4.0.14, Digest: 'sha256:21c5dcdc2b06bf8fdfb998035130d27debf060046e4fa0ed464a6d8aed3ba234', Build: '0', Tool: '0' }
		'mwasplund|Opal': { Version: 0.12.4, Digest: 'sha256:434f3d48248033b798961c6047a48106d5a78407a9730958516a0090f7167e68', Build: '0', Tool: '0' }
		'mwasplund|Soup.Test.Assert': { Version: 0.5.0, Digest: 'sha256:54b1d3506ee764936c0ba81388853854bfd328b6e7292e2486212f5c8596b570', Build: '0', Tool: '0' }
		'mwasplund|reflex': { Version: 5.5.2, Digest: 'sha256:1b49b3cd0ff90bd4f9241f0d00464e8de92dc0454301b0c443297dfe02fac3e6', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'Soup|Cpp': {
				Version: 0.16.1
				Digest: 'sha256:ce0560613b64d81758e0c3be4c8872da654940799eaa3b4c59ed2ef8e1700737'
				Artifacts: {
					Linux: 'sha256:839b38ef0586fff071e4101429ce751431eefa2a499d6b8b5f9f92349a817e1d'
				}
			}
		}
	}
	'1': {
		Wren: {
			'Soup|Cpp': {
				Version: 0.16.1
				Digest: 'sha256:ce0560613b64d81758e0c3be4c8872da654940799eaa3b4c59ed2ef8e1700737'
				Artifacts: {
					Linux: 'sha256:839b38ef0586fff071e4101429ce751431eefa2a499d6b8b5f9f92349a817e1d'
				}
			}
			'mwasplund|Soup.Test.Cpp': {
				Version: 0.15.2
				Digest: 'sha256:7edd5e44357e50dd8a1ccc6f82d5d86f1d164f26810548b73cadbca91308fce1'
			}
		}
	}
	'2': {
		Wren: {
			'Soup|C': {
				Version: 0.5.0
				Digest: 'sha256:1e9683087b84ed998a5ea4a96ee70561e4af5575f46c68d4cdf8a154d405d52b'
				Artifacts: {
					Linux: 'sha256:78475282211521684dd619b99b2cce6edb3034192209aa86317aec2b26a0f207'
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
				Digest: 'sha256:d493afdc0eba473a7f5a544cc196476a105556210bc18bd6c1ecfff81ba07290'
				Artifacts: {
					Linux: 'sha256:cd2e05f53f8e6515383c6b5b5dc6423bda03ee9d4efe7bd2fa74f447495471d2'
				}
			}
			'mwasplund|mkdir': {
				Version: 1.2.0
				Digest: 'sha256:b423f7173bb4eb233143f6ca7588955a4c4915f84945db5fb06ba2eec3901352'
				Artifacts: {
					Linux: 'sha256:bbf3cd98e44319844de6e9f21de269adeb0dabf1429accad9be97f3bd6c56bbd'
				}
			}
			'mwasplund|parse.modules': {
				Version: 1.2.1
				Digest: 'sha256:0e1d4505c320d846008976b9ad4e6e1144c495b08a97979be68c64379b0c4ae5'
				Artifacts: {
					Linux: 'sha256:b3670d71a95093b37fc76b4f8908bd03085de79481e14796aadfae43e67fad64'
				}
			}
		}
	}
}