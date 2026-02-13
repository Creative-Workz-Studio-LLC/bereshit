// ═══════════════════════════════════════════════════════════════════════════
// METADATA
// ═══════════════════════════════════════════════════════════════════════════
//
// Key: cws-ops-dns
// Purpose: Cloudflare DNS management via API v4 (net/http, no SDK)
// Biblical: Joshua 18:4 — "describe the land"
// Authors: Nova Dawn
// Version: 1.0.0
// Created: 2026-02-12
//
// ═══════════════════════════════════════════════════════════════════════════

package ops

// ═══════════════════════════════════════════════════════════════════════════
// SETUP
// ═══════════════════════════════════════════════════════════════════════════

import (
	"bytes"
	"context"
	"encoding/json"
	"fmt"
	"io"
	"net/http"
)

// ═══════════════════════════════════════════════════════════════════════════
// BODY
// ═══════════════════════════════════════════════════════════════════════════

const cfAPI = "https://api.cloudflare.com/client/v4"

// DNSRecord represents a Cloudflare DNS record.
type DNSRecord struct {
	ID      string `json:"id"`
	Name    string `json:"name"`
	Type    string `json:"type"`
	Content string `json:"content"`
	Proxied bool   `json:"proxied"`
	TTL     int    `json:"ttl"`
}

// CloudflareClient provides DNS operations against the Cloudflare API.
type CloudflareClient struct {
	token  string
	zoneID string
}

// NewCloudflareClient creates a client for DNS operations.
// Token is the API token with DNS:Edit permissions.
// ZoneID is the zone identifier for the domain.
func NewCloudflareClient(token, zoneID string) *CloudflareClient {
	return &CloudflareClient{token: token, zoneID: zoneID}
}

// ListDNSRecords returns all DNS records for the zone.
func (c *CloudflareClient) ListDNSRecords(ctx context.Context) ([]DNSRecord, error) {
	url := fmt.Sprintf("%s/zones/%s/dns_records?per_page=100", cfAPI, c.zoneID)

	body, err := c.doRequest(ctx, "GET", url, nil)
	if err != nil {
		return nil, err
	}

	var resp struct {
		Result  []DNSRecord `json:"result"`
		Success bool        `json:"success"`
	}
	if err := json.Unmarshal(body, &resp); err != nil {
		return nil, fmt.Errorf("parse response: %w", err)
	}
	if !resp.Success {
		return nil, fmt.Errorf("cloudflare API error: %s", string(body))
	}

	return resp.Result, nil
}

// UpdateDNSRecord updates the content of an existing DNS record by ID.
func (c *CloudflareClient) UpdateDNSRecord(ctx context.Context, recordID string, rec DNSRecord) error {
	url := fmt.Sprintf("%s/zones/%s/dns_records/%s", cfAPI, c.zoneID, recordID)

	payload, err := json.Marshal(map[string]any{
		"type":    rec.Type,
		"name":    rec.Name,
		"content": rec.Content,
		"proxied": rec.Proxied,
		"ttl":     rec.TTL,
	})
	if err != nil {
		return err
	}

	body, err := c.doRequest(ctx, "PUT", url, payload)
	if err != nil {
		return err
	}

	var resp struct {
		Success bool `json:"success"`
	}
	if err := json.Unmarshal(body, &resp); err != nil {
		return fmt.Errorf("parse response: %w", err)
	}
	if !resp.Success {
		return fmt.Errorf("cloudflare API error: %s", string(body))
	}

	return nil
}

// doRequest executes an authenticated Cloudflare API request.
func (c *CloudflareClient) doRequest(ctx context.Context, method, url string, payload []byte) ([]byte, error) {
	var bodyReader io.Reader
	if payload != nil {
		bodyReader = bytes.NewReader(payload)
	}

	req, err := http.NewRequestWithContext(ctx, method, url, bodyReader)
	if err != nil {
		return nil, err
	}

	req.Header.Set("Authorization", "Bearer "+c.token)
	req.Header.Set("Content-Type", "application/json")

	resp, err := http.DefaultClient.Do(req)
	if err != nil {
		return nil, fmt.Errorf("cloudflare request failed: %w", err)
	}
	defer resp.Body.Close()

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		return nil, fmt.Errorf("read response: %w", err)
	}

	if resp.StatusCode >= 400 {
		return body, fmt.Errorf("cloudflare API %d: %s", resp.StatusCode, string(body))
	}

	return body, nil
}

// ═══════════════════════════════════════════════════════════════════════════
// CLOSING
// ═══════════════════════════════════════════════════════════════════════════
