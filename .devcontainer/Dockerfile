FROM mcr.microsoft.com/devcontainers/universal:4.1.1-linux
RUN apt-get update

# ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

# # Optionally install the cmake for vcpkg
# COPY ./reinstall-cmake.sh /tmp/

# RUN if [ "${REINSTALL_CMAKE_VERSION_FROM_SOURCE}" != "none" ]; then \
#     chmod +x /tmp/reinstall-cmake.sh && /tmp/reinstall-cmake.sh ${REINSTALL_CMAKE_VERSION_FROM_SOURCE}; \
#     fi \
#     && rm -f /tmp/reinstall-cmake.sh

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>

# Install Soup
RUN wget https://github.com/soup-build/soup/releases/latest/download/soup-build-amd64.deb && apt install ./soup-build-amd64.deb && rm soup-build-amd64.deb
RUN swhere