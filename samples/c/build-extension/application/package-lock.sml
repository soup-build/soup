Version: 6
Closure: {
	C: {
		'samples-c-build-extension-application': { Version: './', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'samples-c-build-extension-extension': { Version: '../extension/' }
			'soup|c': {
				Version: 0.9.3
				Digest: 'sha256:023fc6b0a29e7e18b10cef32b35774c8d5b5c53a8fd58fda1879f36c73cfc0f1'
				Artifacts: {
					Linux: 'sha256:919b5abd04c906997f6506fa949cd5f5bfcaee189bf1832e5fefaedfbd1722fc'
					Windows: 'sha256:ee746878b7393bcaa789f619c8b120909244376e36f57cdfbb8498499ff7441d'
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
}