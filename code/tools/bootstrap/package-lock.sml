Version: 6
Closure: {
	C: {
		'mwasplund|libseccomp': { Version: 2.5.9, Digest: 'sha256:13d4dd00af9758bacffac95d987071ecbdfa994de2d86e331dd7f81318bed243', Build: '4', Tool: '0' }
		'mwasplund|wren': { Version: 1.2.0, Digest: 'sha256:6b867a0b48ad53165921bc734032dc245b045898683e744c4df74c2ee06faecc', Build: '4', Tool: '0' }
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
		'Soup.Core': { Version: '../../client/core/', Build: '3', Tool: '1' }
		'Soup.Native': { Version: '../../client/native/', Build: '2', Tool: '1' }
		'mwasplund|CryptoPP': { Version: 1.2.7, Digest: 'sha256:354692b27aae43193129cf3607e3ced863f980083de84dddc0a7d6203f471e45', Build: '2', Tool: '1' }
		'mwasplund|Detours': { Version: 4.0.14, Digest: 'sha256:21c5dcdc2b06bf8fdfb998035130d27debf060046e4fa0ed464a6d8aed3ba234', Build: '2', Tool: '1' }
		'mwasplund|Opal': { Version: 0.12.4, Digest: 'sha256:434f3d48248033b798961c6047a48106d5a78407a9730958516a0090f7167e68', Build: '2', Tool: '1' }
		'mwasplund|Soup.Test.Assert': { Version: 0.5.0, Digest: 'sha256:54b1d3506ee764936c0ba81388853854bfd328b6e7292e2486212f5c8596b570', Build: '2', Tool: '1' }
		'mwasplund|json11': { Version: 1.1.5, Digest: 'sha256:f646ce4a624262be6608142c1df17e72480ec413c36c100eb6b44257d09173da', Build: '2', Tool: '1' }
		'mwasplund|reflex': { Version: 5.5.2, Digest: 'sha256:1b49b3cd0ff90bd4f9241f0d00464e8de92dc0454301b0c443297dfe02fac3e6', Build: '2', Tool: '1' }
	}
}
Builds: {
	'0': {
		Wren: {
			'Soup|CSharp': {
				Version: 0.17.0
				Digest: 'sha256:5f8a05aedfc9d1c6857213367c70febd008b3d5e8c98f510e898177676ebd65a'
				Artifacts: {
					Linux: 'sha256:ce00f1900d5493fc7210427f6c3efe0dfbeb09ce8a7429af845d7cf478f475b4'
				}
			}
		}
	}
	'1': {
		Wren: {
			'Soup|CSharp': {
				Version: 0.17.0
				Digest: 'sha256:5f8a05aedfc9d1c6857213367c70febd008b3d5e8c98f510e898177676ebd65a'
				Artifacts: {
					Linux: 'sha256:ce00f1900d5493fc7210427f6c3efe0dfbeb09ce8a7429af845d7cf478f475b4'
				}
			}
			'Soup|CSharp.Nuget': {
				Version: 0.7.0
				Digest: 'sha256:36014a673aeb99acc53f46b25011f5859e36631d6a195ca887722093cd047c26'
			}
		}
	}
	'2': {
		Wren: {
			'Soup|Cpp': {
				Version: 0.16.1
				Digest: 'sha256:ce0560613b64d81758e0c3be4c8872da654940799eaa3b4c59ed2ef8e1700737'
				Artifacts: {
					Linux: 'sha256:2accd63cb5e791898174789694dd3e7479eb04d59761ef749858a5eb78a9fcf5'
					Windows: 'sha256:bca32ccccf13a3f77bdcf3f4f81167642b55ffc987b810a91d7c10d5aa03030b'
				}
			}
		}
	}
	'3': {
		Wren: {
			'Soup|Cpp': {
				Version: 0.16.1
				Digest: 'sha256:ce0560613b64d81758e0c3be4c8872da654940799eaa3b4c59ed2ef8e1700737'
				Artifacts: {
					Linux: 'sha256:2accd63cb5e791898174789694dd3e7479eb04d59761ef749858a5eb78a9fcf5'
					Windows: 'sha256:bca32ccccf13a3f77bdcf3f4f81167642b55ffc987b810a91d7c10d5aa03030b'
				}
			}
			'mwasplund|Soup.Test.Cpp': {
				Version: 0.15.2
				Digest: 'sha256:7edd5e44357e50dd8a1ccc6f82d5d86f1d164f26810548b73cadbca91308fce1'
			}
		}
	}
	'4': {
		Wren: {
			'Soup|C': {
				Version: 0.6.1
				Digest: 'sha256:aeef46ffb39aa3f46e39967b0762106e2554aacb299638b83198a812e5a818b0'
				Artifacts: {
					Linux: 'sha256:8c3b9a842cdc84a3a27ed8eff8258529eb8f78c215d82b4faf00bf1bda55a814'
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
					Windows: 'sha256:c4dc68326a11a704d568052e1ed46bdb3865db8d12b7d6d3e8e8d8d6d3fad6c8'
				}
			}
			'mwasplund|mkdir': {
				Version: 1.2.0
				Digest: 'sha256:b423f7173bb4eb233143f6ca7588955a4c4915f84945db5fb06ba2eec3901352'
				Artifacts: {
					Linux: 'sha256:bbf3cd98e44319844de6e9f21de269adeb0dabf1429accad9be97f3bd6c56bbd'
					Windows: 'sha256:4d43a781ed25ae9a97fa6881da7c24425a3162703df19964d987fb2c7ae46ae3'
				}
			}
		}
	}
	'1': {
		'C++': {
			'mwasplund|copy': {
				Version: 1.2.0
				Digest: 'sha256:d493afdc0eba473a7f5a544cc196476a105556210bc18bd6c1ecfff81ba07290'
				Artifacts: {
					Linux: 'sha256:cd2e05f53f8e6515383c6b5b5dc6423bda03ee9d4efe7bd2fa74f447495471d2'
					Windows: 'sha256:c4dc68326a11a704d568052e1ed46bdb3865db8d12b7d6d3e8e8d8d6d3fad6c8'
				}
			}
			'mwasplund|mkdir': {
				Version: 1.2.0
				Digest: 'sha256:b423f7173bb4eb233143f6ca7588955a4c4915f84945db5fb06ba2eec3901352'
				Artifacts: {
					Linux: 'sha256:bbf3cd98e44319844de6e9f21de269adeb0dabf1429accad9be97f3bd6c56bbd'
					Windows: 'sha256:4d43a781ed25ae9a97fa6881da7c24425a3162703df19964d987fb2c7ae46ae3'
				}
			}
			'mwasplund|parse.modules': {
				Version: 1.2.1
				Digest: 'sha256:0e1d4505c320d846008976b9ad4e6e1144c495b08a97979be68c64379b0c4ae5'
				Artifacts: {
					Linux: 'sha256:b3670d71a95093b37fc76b4f8908bd03085de79481e14796aadfae43e67fad64'
					Windows: 'sha256:74bbd90feaee02a24cb08d5056f6b9960b6b7ecdfb375d24e0bc27b14b2a6eae'
				}
			}
		}
	}
}