Version: 6
Closure: {
	C: {
		'samples-interop-cpp-to-c-library': { Version: '../library/', Build: '1', Tool: '0' }
	}
	'C++': {
		'samples-interop-cpp-to-c-application': { Version: './', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'soup|cpp': {
				Version: 0.19.0
				Digest: 'sha256:cfc5518b0d904bf568e484e8c834438f2f4206dc3efbd2a889abc9f6ceedd2eb'
				Artifacts: {
					Linux: 'sha256:95611f04a2ed1f158b33fbc97a9a253af6152eb09a12935eabf2e51c09ce0595'
				}
			}
		}
	}
	'1': {
		Wren: {
			'soup|c': {
				Version: 0.8.0
				Digest: 'sha256:8e816fc36d390ea692f29d799dd2f803e8ef614e685ef33cd9c869019a80f9f2'
				Artifacts: {
					Linux: 'sha256:7625f2e86e62c08279a7daf63ccc0638040f2b0eda3b508d375da6f974c509a0'
					Windows: 'sha256:ab62a89fe58c38b9805d6dc65a107e16aa16adb5033604800ff58a37623c77f9'
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