Version: 6
Closure: {
	'C#': {
		'samples-interop-csharp-to-cpp-application': { Version: './', Build: '0', Tool: '0' }
	}
	'C++': {
		'samples-interop-csharp-to-cpp-library': { Version: '../library/', Build: '1', Tool: '1' }
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
			'soup|cpp': {
				Version: 0.19.2
				Digest: 'sha256:fbb82059675361bc31d6ce29fa17ec42320d173a1267230fbf35ad1b0dcb46d0'
				Artifacts: {
					Linux: 'sha256:eb5cc7691bff09688a568c3db9a21e549fd42cfb207500fcafd4fe4f3509ccc0'
					Windows: 'sha256:ba56f2b3cf9dc2e439e48a16a04ec660c5122d87eb31a0959fd6718f38fa7e36'
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
			'mwasplund|parse-modules': {
				Version: 3.0.0
				Digest: 'sha256:c7d7b9e7de3b304c936430be8d94b406b40939550503c0d8b5d3c1a5d10815e2'
				Artifacts: {
					Linux: 'sha256:8a54de2c0495c08349c7f9e073536c30c071cca9c311639c59b10bf11c3ebe79'
					Windows: 'sha256:ce5ab795beffde62d46d00a24a20b5206fa6e560a05c8a5058141aa8bd7e64ad'
				}
			}
		}
	}
}