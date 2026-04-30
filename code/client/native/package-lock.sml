Version: 6
Closure: {
	C: {
		'mwasplund|libseccomp': { Version: 2.5.9, Digest: 'sha256:13d4dd00af9758bacffac95d987071ecbdfa994de2d86e331dd7f81318bed243', Build: '2', Tool: '0' }
		'mwasplund|wren': { Version: 1.2.0, Digest: 'sha256:6b867a0b48ad53165921bc734032dc245b045898683e744c4df74c2ee06faecc', Build: '2', Tool: '0' }
	}
	'C++': {
		'monitor-host': { Version: '../../monitor/host/', Build: '0', Tool: '0' }
		'monitor-shared': { Version: '../../monitor/shared/', Build: '0', Tool: '0' }
		'mwasplund|cryptopp': { Version: 1.2.8, Digest: 'sha256:6d19806db25880437c805a616a3470ddb1b86cba2e6bfd83a378118e1f46e037', Build: '0', Tool: '0' }
		'mwasplund|detours': { Version: 4.0.14, Digest: 'sha256:042d256bb8aecc3c5b58ff5be26c3f21a3ba24de5885663fb7d6a928c18ebfc4', Build: '0', Tool: '0' }
		'mwasplund|json11': { Version: 1.1.6, Digest: 'sha256:c6b0981921f926b73e9512d068efde6eb2c5183f6b3a8442bddc67f847d2bfc4', Build: '0', Tool: '0' }
		'mwasplund|opal': { Version: 0.13.0, Digest: 'sha256:7aefec36342b2f3112e3ebf4f1fd1e11153b1a72742506d3b96ac569b03df9cb', Build: '0', Tool: '0' }
		'mwasplund|reflex': { Version: 5.5.4, Digest: 'sha256:09e879b8c200c6415543686cf81c710712d07286753cdcd77cec08eab8e81bbe', Build: '0', Tool: '0' }
		'mwasplund|soup-test-assert': { Version: 0.5.0, Digest: 'sha256:ae069ae6775ed31b5eb3ba5c05f33fd8e786cb77884991ae642b887cf7e0dde0', Build: '0', Tool: '0' }
		sml: { Version: '../../sml/', Build: '0', Tool: '0' }
		'soup-core': { Version: '../core/', Build: '1', Tool: '0' }
		'soup-native': { Version: './', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'soup|cpp': {
				Version: 0.19.4
				Digest: 'sha256:24cf2167fb91e85589d242aadfd04d4f81b6388248d959557dc10642c08a0cee'
				Artifacts: {
					Linux: 'sha256:b9d96a131b2b521fee79f8eec8af5b49c5361a3a6b222e49f9daa6d0851ae505'
					Windows: 'sha256:20bb37ee5ec324df254d039ba34b713d54f046e8c2f89e5e1c96ef7b773ad8b9'
				}
			}
		}
	}
	'1': {
		Wren: {
			'soup|cpp': {
				Version: 0.19.4
				Digest: 'sha256:24cf2167fb91e85589d242aadfd04d4f81b6388248d959557dc10642c08a0cee'
				Artifacts: {
					Linux: 'sha256:b9d96a131b2b521fee79f8eec8af5b49c5361a3a6b222e49f9daa6d0851ae505'
					Windows: 'sha256:20bb37ee5ec324df254d039ba34b713d54f046e8c2f89e5e1c96ef7b773ad8b9'
				}
			}
			'mwasplund|soup-test-cpp': {
				Version: 0.18.1
				Digest: 'sha256:abeba7430c29089dbb24b932779f231b5f443203772579807067437e34cc4e55'
			}
		}
	}
	'2': {
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