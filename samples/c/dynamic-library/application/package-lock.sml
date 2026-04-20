Version: 6
Closure: {
	C: {
		'samples-c-dynamic-library-application': { Version: './', Build: '0', Tool: '0' }
		'samples-c-dynamic-library-library': { Version: '../library/', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'soup|c': {
				Version: 0.9.2
				Digest: 'sha256:0265ede1ffee044b0e008e798b8a91da003ab59bf9a601df595229c870abc167'
				Artifacts: {
					Linux: 'sha256:ccb7c2a64eb5650468931fe091ecd1b8de38ea4f776c14dc5c6ac255f95a01d7'
					Windows: 'sha256:e3946263dc70369ffa82678beff0747366fc64ff2bafad4b432f0a3ce78e39dc'
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