Version: 6
Closure: {
	'C#': {
		'soup-utilities': { Version: '../utilities/', Build: '1', Tool: '0' }
		Swhere: { Version: './', Build: '0', Tool: '0' }
		opal: { Version: '../opal/', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'soup|csharp': {
				Version: 0.18.1
				Digest: 'sha256:473d683e9e0157cf695f47f54e5ded7549c0885effe51ad891aa4b9ff7bc63e0'
				Artifacts: {
					Windows: 'sha256:54465203300ca0a74c2bf311e51b2cb7c0740905bea4168a59e26f4653d36023'
				}
			}
		}
	}
	'1': {
		Wren: {
			'soup|csharp': {
				Version: 0.18.1
				Digest: 'sha256:473d683e9e0157cf695f47f54e5ded7549c0885effe51ad891aa4b9ff7bc63e0'
				Artifacts: {
					Windows: 'sha256:54465203300ca0a74c2bf311e51b2cb7c0740905bea4168a59e26f4653d36023'
				}
			}
			'Soup|CSharp.Nuget': {
				Version: 0.9.1
				Digest: 'sha256:36014a673aeb99acc53f46b25011f5859e36631d6a195ca887722093cd047c26'
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