Version: 6
Closure: {
	'C++': {
		'Monitor.Client': { Version: './', Build: '0', Tool: '0' }
		'Monitor.Shared': { Version: '../shared/', Build: '0', Tool: '0' }
		'mwasplund|Detours': { Version: 4.0.14, Digest: 'sha256:21c5dcdc2b06bf8fdfb998035130d27debf060046e4fa0ed464a6d8aed3ba234', Build: '0', Tool: '0' }
		'mwasplund|Opal': { Version: 0.12.4, Digest: 'sha256:434f3d48248033b798961c6047a48106d5a78407a9730958516a0090f7167e68', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
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