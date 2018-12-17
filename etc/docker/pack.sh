#!/bin/bash
set -e

function docker::build_local {
    tag=$1
    dockerfile=$2
    folder=$3
    echo "!! Building '${tag}'"
    sleep 2
    docker build ${folder} -f ${dockerfile} -t ${tag} --network=host
}

function docker::push_image {
    local_tag=$1
    remote_tag=${docker_user}/$2
    read -r -p "?? Do you want to push image ${remote_tag}? [y/N] " response
    if [[ "$response" =~ ^([yY][eE][sS]|[yY])+$ ]]; then
        docker tag ${local_tag} ${remote_tag}
        echo "!! Log-in as '${docker_user}' at Docker registry:"
        docker login -u ${docker_user}
        docker push ${remote_tag}
    fi
}

image_dev='is-cameras/dev'
docker_user="viros"
remote_tag="is-cameras:1.5.1"

docker::build_local is-cameras Dockerfile
docker::push_image is-cameras ${remote_tag}