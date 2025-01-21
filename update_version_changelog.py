import re
import os
import requests
import subprocess

# Fetch the current version from version.h
def get_current_version():
    with open("version.h", "r") as file:
        content = file.read()
    match = re.search(r'#define VERSION "(\d+\.\d+\.\d+)"', content)
    if match:
        return match.group(1)
    return None

# Update the version in version.h
def update_version(current_version):
    new_version = increment_version(current_version)
    while release_tag_exists(new_version):
        new_version = increment_version(new_version)
    with open("version.h", "r") as file:
        content = file.read()
    new_content = re.sub(r'#define VERSION "\d+\.\d+\.\d+"', f'#define VERSION "{new_version}"', content)
    with open("version.h", "w") as file:
        file.write(new_content)
    return new_version


# Increment the version number
def increment_version(version):
    major, minor, patch = map(int, version.split('.'))
    patch += 1
    return f"{major}.{minor}.{patch}"

# Check if the release tag already exists
def release_tag_exists(version):
    result = subprocess.run(['git', 'tag', '-l', f'v{version}'], capture_output=True, text=True)
    return f'v{version}' in result.stdout

# Fetch issue summaries from YouTrack
def get_issue_summaries(commit_messages):
    issues = {}
    for message in commit_messages:
        match = re.search(r'TBX-(\d+)', message)
        if match:
            issue_id = match.group(1)
            response = requests.get(f"https://kb.mypdns.org/api/issues/TBX-{issue_id}", headers={
                "Authorization": f"Bearer {os.getenv('KB_YOUTRACK_API_TOKEN')}"
            })
            if response.status_code == 200:
                issue_data = response.json()
                issues[issue_id] = issue_data.get('summary', 'No summary available')
    return issues

# Update the CHANGELOG
def update_changelog(new_version, issues):
    with open("CHANGELOG.md", "r") as file:
        content = file.read()
    new_content = f"## Release {new_version}\n"
    if issues:
        for issue_id, summary in issues.items():
            new_content += f"- [TBX-{issue_id}](https://kb.mypdns.org/issues/TBX-{issue_id}): {summary}\n"
    else:
        new_content += "- No comments, minor update\n"
    new_content += "\n" + content
    with open("CHANGELOG.md", "w") as file:
        file.write(new_content)

# Main function
def main():
    current_version = get_current_version()
    if not current_version:
        print("Could not find the current version.")
        return
    new_version = update_version(current_version)

    # Fetch the latest commit messages
    commit_messages = os.popen('git log --format=%B -n 5').read().strip().split('\n')

    # Get issue summaries from YouTrack
    issues = get_issue_summaries(commit_messages)

    # Update the CHANGELOG
    update_changelog(new_version, issues)

    print(f"Updated to version {new_version}")

if __name__ == "__main__":
    main()