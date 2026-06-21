Version: 6
Closure: {
	'C++': {
		'samples-cpp-windows-application': { Version: './', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'soup|cpp': {
				Version: 0.20.0
				Digest: 'sha256:f4a6bd667a431c011d1a2eb298918db58e05a2d8d8186afea041d51ff1ecee26'
				Artifacts: {
					Windows: 'sha256:22dd63fbe71ecf70db8c229f5d0b8ee355dd912ff7f81e7a0a11b5d7ae534e4d'
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
			'mwasplund|parse-modules': {
				Version: 4.0.0
				Digest: 'sha256:85318f523aac6635109a51f7c0bddcc44fff28da4b7613514db6cefb3ca8ceee'
				Artifacts: {
					Linux: 'sha256:f71f90238478d1463efb621e18ac7da091e55684451db89da40b70fba340a872'
					Windows: 'sha256:6e7b764ca856d6b5c97db3fe22e5442a755f7f56089b0faac550bc216c43f1a8'
				}
			}
		}
	}
}