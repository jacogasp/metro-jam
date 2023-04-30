import urllib3
import json
import subprocess
import os
import zipfile
import sys


CACHE_FILE = "build/.artifacts_cache"
_, REPO = sys.argv

url = f"https://api.github.com/repos/{REPO}/actions/artifacts"
print(url)
http = urllib3.PoolManager()


def get_artifacts_list():
    r = http.request("GET", url)
    data = r.data.decode("utf-8")
    return json.loads(data)["artifacts"]


def get_latests(artifacts: list):
    artifacts.sort(key=lambda art: art["updated_at"], reverse=True)
    latests = {}

    for artifact in artifacts:
        name = artifact["name"]
        if name not in latests:
            latests[name] = artifact

    return latests


def load_cache() -> dict:
    if os.path.isfile(CACHE_FILE):
        print("Load artifacts cache file")
        with open(CACHE_FILE, 'r') as f:
            return json.load(f)

    print("Artifacts cache file not found.")


def save_cache(buf: str):
    with open(CACHE_FILE, 'w') as f:
        i = f.write(buf)
        print(f"Saved {i} bytes to {CACHE_FILE}")


def check_cache(cache, latests: dict) -> dict:
    if cache is None:
        return latests

    to_download = {}

    for a_lst_name, a_lst in latests.items():
        if a_lst_name in cache and a_lst["id"] == cache[a_lst_name]["id"]:
            continue
        to_download[a_lst_name] = a_lst

    return to_download


def print_download_list(artifacts):
    print("Artifacts to download:")
    for name in artifacts.keys():
        print(f" - {name}")


def get_file_path_and_dir(artifact):
    name = artifact["name"]
    ext = name.split(".")[-1]

    build_dir_map = {
        "so": "linux",
        "dylib": "macos",
        "dll": "windows"
    }

    output_dir = os.path.join("build", build_dir_map[ext])

    if not os.path.isdir(output_dir):
        os.makedirs(output_dir)

    output_path = os.path.join(output_dir, name + ".zip")
    return output_path, output_dir


def download_artifact(artifact, output_path):
    name = artifact["name"]
    cmd = [
        'gh', 'api',
        '-H', 'Accept: application/vnd.github+json',
        '-H', 'X-GitHub-Api-Version: 2022-11-28',
        f'{artifact["url"]}/zip'
    ]
    print(f"Download artifact {name}")
    with open(output_path, 'wb') as f:
        subprocess.call(cmd, stdout=f)


def unzip_artifact(path, output_dir):
    print("Unzip", path)
    with zipfile.ZipFile(path, 'r') as z:
        z.extractall(output_dir)
    os.remove(path)


def download_artifacts(artifacts: dict):
    for artifact in artifacts.values():
        path, output_dir = get_file_path_and_dir(artifact)
        download_artifact(artifact, path)
        unzip_artifact(path, output_dir)


def main():
    artifacts = get_artifacts_list()
    latests = get_latests(artifacts)
    cache = load_cache()
    to_download = check_cache(cache, latests)

    if len(to_download) > 0:
        print_download_list(to_download)
        download_artifacts(to_download)
        save_cache(json.dumps(latests))
    else:
        print("Artifacts are up to date")


if __name__ == "__main__":
    main()
