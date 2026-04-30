Version: 6
Closure: {
	C: {
		'samples-interop-csharp-to-c-library': { Version: '../library/', Build: '1', Tool: '0' }
	}
	'C#': {
		'samples-interop-csharp-to-c-application': { Version: './', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'soup|csharp': {
				Version: 0.18.1
				Digest: 'sha256:473d683e9e0157cf695f47f54e5ded7549c0885effe51ad891aa4b9ff7bc63e0'
				Artifacts: {
					Linux: 'sha256:8b9d8cd7aba4d4edbb0d2d8fb23344ba42d0db70b1ee792c69ae67a6e2ddadbe'
					Windows: 'sha256:54465203300ca0a74c2bf311e51b2cb7c0740905bea4168a59e26f4653d36023'
				}
			}
		}
	}
	'1': {
		Wren: {
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