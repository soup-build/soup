Version: 6
Closure: {
	'C++': {
		mkdir: { Version: './', Build: '0', Tool: '0' }
		'mwasplund|opal': { Version: 0.14.0, Digest: 'sha256:b211995f3b85ec2a2895f9f4d541b75ddb09ccd542943ae2391d8fdfbe3c16f2', Build: '0', Tool: '0' }
	}
}
Builds: {
	'0': {
		Wren: {
			'soup|cpp': {
				Version: 0.20.3
				Digest: 'sha256:01090a3637e82e3a428175f4a5c562cbbd87488b4ac26a7bd0b5b9f3706ebd57'
				Artifacts: {
					Linux: 'sha256:e8fe01767098cbf13c90a447bbb45da3c44be0bf30c159f1d457f7d196e47862'
					Windows: 'sha256:cb2bd1824c6a70077b93d98d56d8f665cc3891eb528fe0415998827e6356c09f'
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
				Version: 4.0.0
				Digest: 'sha256:85318f523aac6635109a51f7c0bddcc44fff28da4b7613514db6cefb3ca8ceee'
				Artifacts: {
					Linux: 'sha256:a1d15fada2827293862377a381427c5e75cbca16afcf913e9fcef6346b087a75'
					Windows: 'sha256:ad30416a9a2dc0eebed727e16c8676ae35c3496a3cbe218885af868df4f55d50'
				}
			}
		}
	}
}