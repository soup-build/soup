Version: 6
Closure: {
	Wren: {
		'Samples.CSharp.BuildExtension.Extension': { Version: './', Build: '0', Tool: '0' }
		'Soup|Build.Utils': { Version: 0.9.2, Digest: 'sha256:0b70e1c4392ee591b06e64b00fc3b267fad1f32ced90e7b2d61c01c2780e4096', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'Soup|Wren': {
				Version: 0.5.4
				Digest: 'sha256:11c795361ffddfb5f954cf747e69c7036d3f52fdd86e0869a40a55c03fa9bac3'
				Artifacts: {
					Linux: 'sha256:61028ebdd655b14bf293febc483d04c5312c3546b7890dfa104e0e1cf6121112'
					Windows: 'sha256:a63c6fa40955a46b0e3b95a13cfceaba1e067229b9ef518b957fa7cabb6c195a'
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