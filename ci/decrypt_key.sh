openssl aes-256-cbc -K $encrypted_f7e980bb1803_key -iv $encrypted_f7e980bb1803_iv -in ci/travis-ci_rsa.enc -out ci/travis-ci_rsa -d
chmod 0600 ci/travis-ci_rsa
cp ci/travis-ci_rsa ~/.ssh/id_rsa
