FROM ubuntu:24.04

# ```
# $ docker build --tag flex:2.6.4-8.2build1 --file .\Dockerfile.flex .
# ```

RUN apt-get update && apt-get install --yes \
    # apt list -a flex
    flex=2.6.4-8.2build1 \
    && rm -rf /var/lib/apt/lists/*

ENTRYPOINT [ "flex" ]