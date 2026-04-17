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
				Version: 0.9.0
				Digest: 'sha256:080c2091ff9ae2ea2fd02866771ca68d4a0509106e6bd31a4a1a87f0f5e1a1c8'
				Artifacts: {
					Linux: 'sha256:8d1d85f9e24acb06e2e0eb7ad433d62970ec58058954d1e8e7bb7f363c5f6521'
					Windows: 'sha256:6f60c4124aa5a0dc917f36a34027b03ac115f0ba533ca541ec30235fda2fa0cf'
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