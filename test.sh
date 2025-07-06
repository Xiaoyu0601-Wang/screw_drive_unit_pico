#!/bin/bash

# GitHub Repository Downloader Script
# Usage: ./github_downloader.sh [OPTIONS] <repository_url_or_owner/repo>
# 
# This script downloads GitHub repositories using various methods:
# - git clone (default)
# - wget/curl for archive downloads
# - GitHub CLI (gh) if available

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default options
CLONE_METHOD="git"
OUTPUT_DIR=""
BRANCH=""
DEPTH=""
ARCHIVE_FORMAT="zip"
QUIET=false
HELP=false

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to show help
show_help() {
    cat << EOF
GitHub Repository Downloader Script

USAGE:
    $0 [OPTIONS] <repository>

ARGUMENTS:
    <repository>    GitHub repository in format 'owner/repo' or full GitHub URL

OPTIONS:
    -m, --method METHOD     Download method: git, archive, gh (default: git)
    -o, --output DIR        Output directory (default: repository name)
    -b, --branch BRANCH     Specific branch to download (default: main/master)
    -d, --depth DEPTH       Clone depth for git method (shallow clone)
    -f, --format FORMAT     Archive format: zip, tar.gz (default: zip, only for archive method)
    -q, --quiet             Quiet mode - minimal output
    -h, --help              Show this help message

EXAMPLES:
    # Clone a repository using git
    $0 torvalds/linux
    
    # Download as archive to specific directory
    $0 -m archive -o ~/Downloads microsoft/vscode
    
    # Shallow clone with specific branch
    $0 -m git -b develop -d 1 facebook/react
    
    # Download using GitHub CLI
    $0 -m gh octocat/Hello-World

METHODS:
    git      - Uses 'git clone' (requires git)
    archive  - Downloads repository archive via wget/curl
    gh       - Uses GitHub CLI (requires 'gh' command)

EOF
}

# Function to parse repository URL/name
parse_repository() {
    local repo_input="$1"
    
    # If it's a full GitHub URL, extract owner/repo
    if [[ "$repo_input" =~ ^https?://github\.com/([^/]+)/([^/]+)/?$ ]]; then
        REPO_OWNER="${BASH_REMATCH[1]}"
        REPO_NAME="${BASH_REMATCH[2]}"
        # Remove .git suffix if present
        REPO_NAME="${REPO_NAME%.git}"
    # If it's in owner/repo format
    elif [[ "$repo_input" =~ ^([^/]+)/([^/]+)$ ]]; then
        REPO_OWNER="${BASH_REMATCH[1]}"
        REPO_NAME="${BASH_REMATCH[2]}"
    else
        print_error "Invalid repository format. Use 'owner/repo' or full GitHub URL."
        exit 1
    fi
    
    REPO_URL="https://github.com/${REPO_OWNER}/${REPO_NAME}.git"
    REPO_ARCHIVE_URL="https://github.com/${REPO_OWNER}/${REPO_NAME}"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to download using git clone
download_with_git() {
    local clone_args=()
    
    if [[ -n "$BRANCH" ]]; then
        clone_args+=("--branch" "$BRANCH")
    fi
    
    if [[ -n "$DEPTH" ]]; then
        clone_args+=("--depth" "$DEPTH")
    fi
    
    if [[ "$QUIET" == true ]]; then
        clone_args+=("--quiet")
    fi
    
    clone_args+=("$REPO_URL")
    
    if [[ -n "$OUTPUT_DIR" ]]; then
        clone_args+=("$OUTPUT_DIR")
    fi
    
    print_info "Cloning repository with git..."
    git clone "${clone_args[@]}"
    print_success "Repository cloned successfully!"
}

# Function to download archive
download_archive() {
    local archive_url
    local output_file
    local extract_dir
    
    # Determine branch
    local branch_param=""
    if [[ -n "$BRANCH" ]]; then
        branch_param="/$BRANCH"
    fi
    
    # Set output directory
    if [[ -n "$OUTPUT_DIR" ]]; then
        extract_dir="$OUTPUT_DIR"
    else
        extract_dir="$REPO_NAME"
    fi
    
    case "$ARCHIVE_FORMAT" in
        "zip")
            archive_url="${REPO_ARCHIVE_URL}/archive/refs/heads${branch_param}.zip"
            output_file="${REPO_NAME}.zip"
            ;;
        "tar.gz")
            archive_url="${REPO_ARCHIVE_URL}/archive/refs/heads${branch_param}.tar.gz"
            output_file="${REPO_NAME}.tar.gz"
            ;;
        *)
            print_error "Unsupported archive format: $ARCHIVE_FORMAT"
            exit 1
            ;;
    esac
    
    print_info "Downloading repository archive..."
    
    # Try wget first, then curl
    if command_exists wget; then
        local wget_args=("-O" "$output_file")
        if [[ "$QUIET" == true ]]; then
            wget_args+=("--quiet")
        fi
        wget "${wget_args[@]}" "$archive_url"
    elif command_exists curl; then
        local curl_args=("-L" "-o" "$output_file")
        if [[ "$QUIET" == true ]]; then
            curl_args+=("--silent")
        fi
        curl "${curl_args[@]}" "$archive_url"
    else
        print_error "Neither wget nor curl is available for downloading archives."
        exit 1
    fi
    
    print_info "Extracting archive..."
    
    # Extract based on format
    case "$ARCHIVE_FORMAT" in
        "zip")
            if command_exists unzip; then
                local unzip_args=()
                if [[ "$QUIET" == true ]]; then
                    unzip_args+=("-q")
                fi
                unzip "${unzip_args[@]}" "$output_file"
                # Rename extracted directory
                extracted_dir=$(unzip -l "$output_file" | head -4 | tail -1 | awk '{print $4}' | cut -d'/' -f1)
                if [[ "$extracted_dir" != "$extract_dir" ]]; then
                    mv "$extracted_dir" "$extract_dir"
                fi
            else
                print_error "unzip command not found. Cannot extract ZIP archive."
                exit 1
            fi
            ;;
        "tar.gz")
            tar -xzf "$output_file"
            # Rename extracted directory
            extracted_dir=$(tar -tzf "$output_file" | head -1 | cut -d'/' -f1)
            if [[ "$extracted_dir" != "$extract_dir" ]]; then
                mv "$extracted_dir" "$extract_dir"
            fi
            ;;
    esac
    
    # Clean up archive file
    rm "$output_file"
    
    print_success "Repository downloaded and extracted successfully!"
}

# Function to download using GitHub CLI
download_with_gh() {
    if ! command_exists gh; then
        print_error "GitHub CLI (gh) is not installed. Please install it first."
        exit 1
    fi
    
    local clone_args=("repo" "clone" "${REPO_OWNER}/${REPO_NAME}")
    
    if [[ -n "$OUTPUT_DIR" ]]; then
        clone_args+=("$OUTPUT_DIR")
    fi
    
    print_info "Downloading repository with GitHub CLI..."
    gh "${clone_args[@]}"
    
    # If branch is specified and it's not the default, switch to it
    if [[ -n "$BRANCH" ]]; then
        local repo_dir="${OUTPUT_DIR:-$REPO_NAME}"
        if [[ -d "$repo_dir" ]]; then
            print_info "Switching to branch: $BRANCH"
            (cd "$repo_dir" && git checkout "$BRANCH")
        fi
    fi
    
    print_success "Repository downloaded successfully with GitHub CLI!"
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -m|--method)
            CLONE_METHOD="$2"
            shift 2
            ;;
        -o|--output)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        -b|--branch)
            BRANCH="$2"
            shift 2
            ;;
        -d|--depth)
            DEPTH="$2"
            shift 2
            ;;
        -f|--format)
            ARCHIVE_FORMAT="$2"
            shift 2
            ;;
        -q|--quiet)
            QUIET=true
            shift
            ;;
        -h|--help)
            show_help
            exit 0
            ;;
        -*)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
        *)
            if [[ -z "$REPOSITORY" ]]; then
                REPOSITORY="$1"
            else
                print_error "Multiple repositories specified. Only one repository is supported."
                exit 1
            fi
            shift
            ;;
    esac
done

# Check if repository is provided
if [[ -z "$REPOSITORY" ]]; then
    print_error "Repository is required."
    show_help
    exit 1
fi

# Parse repository information
parse_repository "$REPOSITORY"

print_info "Repository: ${REPO_OWNER}/${REPO_NAME}"
print_info "Method: $CLONE_METHOD"

# Validate method and required tools
case "$CLONE_METHOD" in
    "git")
        if ! command_exists git; then
            print_error "Git is not installed. Please install git or use a different method."
            exit 1
        fi
        download_with_git
        ;;
    "archive")
        download_archive
        ;;
    "gh")
        download_with_gh
        ;;
    *)
        print_error "Invalid download method: $CLONE_METHOD"
        print_error "Supported methods: git, archive, gh"
        exit 1
        ;;
esac

print_success "Download completed!"