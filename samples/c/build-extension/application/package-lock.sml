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
				Version: 0.9.1
				Digest: 'sha256:77199692633052c889a75d1733462c2d35bb12f204aad94b315990f39a9a5834'
				Artifacts: {
					Linux: 'sha256:622af8221c24d73985c70e20ca24300955d7ceaa6e829bfaa274dadd1e479c6a'
					Windows: 'sha256:1696fc024a6fdd0661fff16b2f5e1f6c956c9c40d597f861e404c27715023037'
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